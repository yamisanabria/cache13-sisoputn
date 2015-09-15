#include "nodo.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <utils.h>
#include <math.h>
#include <socket.h>

// DEFINES

#define BLOCK_SIZE 1024 * 1024 * 20 //Define el tamaño de un bloque (20MB)
#define BACKLOG 5 // Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024 // Define cual va a ser el size maximo del paquete a enviar

// VARIABLES GLOBALES

char *fileConfig = "config.cfg";     /* Ruta a archivo config por defecto*/
char *logPath;						 /* Ruta a archivo loging por defecto*/
int   fsPort;                        /* Puerto donde el proceso FileSystem está  escuchando nuevas conexiones*/
char *fsIP;                          /* Dirección IP de la computadora que está ejecutando el proceso FileSystem*/
char *dataBin;                       /* Nombre del archivo que contiene los bloques de datos*/
char *dirTemp;                       /* Directorio donde se van a almacenar los archivos temporales*/
char *dirScriptsMap;                 /* Directorio donde se van a almacenar los scripts de mapping*/
char *dirScriptsRed;                 /* Directorio donde se van a almacenar los scripts de reduce*/
char *nodoNuevo;                     /* Un nodo nuevo es aquel que nunca formó parte del cluster*/
char *nodoIP;                        /* Dirección IP de la computadora donde está ejecutando un proceso Nodo*/
int   nodoPort;                      /* Puerto en el cual este proceso Nodo espera recibir conexiones nuevas*/
int   idNodo;                        /* Identificador único que nos provee MaRTA para saber que nodo somos */
char *sort = "sort";                 /* sort bash ignorando espacios en blanco */
int   blocks;                        /* Cantidad de Bloques que posee el espacio de datos */
t_dictionary   *fns;                 /* Funciones de socket */
t_log*		   logg;                 /* Variable para logs de Nodo */
pthread_rwlock_t *semBloques;		 /* Mutex para los bloques */
pthread_mutex_t mx_main;             /* Mutex para el proceso main */

// FUNCIONES PROPIAS

void readFileConfig() {

	/* Si la ruta del archivo de configuración, pasada por argumento
	 * es distinta que la preconfigurada, la cambio..  */

	t_config* config;
	config = config_create(fileConfig);

	if (config_has_property(config, "PUERTO_FS"))
		fsPort = config_get_int_value(config, "PUERTO_FS");

	if (config_has_property(config, "IP_FS"))
		fsIP = string_duplicate(config_get_string_value(config, "IP_FS"));

	if (config_has_property(config, "ARCHIVO_BIN"))
		dataBin = string_duplicate(config_get_string_value(config, "ARCHIVO_BIN"));

	if (config_has_property(config, "DIR_TMP"))
		dirTemp = string_duplicate(config_get_string_value(config, "DIR_TMP"));

	if (config_has_property(config, "DIR_SCRIPTS_M"))
		dirScriptsMap = string_duplicate(config_get_string_value(config, "DIR_SCRIPTS_M"));

	if (config_has_property(config, "DIR_SCRIPTS_R"))
		dirScriptsRed = string_duplicate(config_get_string_value(config, "DIR_SCRIPTS_R"));

	if (config_has_property(config, "NODO_ID"))
		idNodo = config_get_int_value(config, "NODO_ID");

	if (config_has_property(config, "IP_NODO"))
		nodoIP = string_duplicate(config_get_string_value(config, "IP_NODO"));

	if (config_has_property(config, "PUERTO_NODO"))
		nodoPort = config_get_int_value(config, "PUERTO_NODO");

	if (config_has_property(config, "LOG"))
		logPath = string_duplicate(config_get_string_value(config, "LOG"));


	config_destroy(config);


}

void getBlocksQuantity(){

		int fd;				/* FileDescriptor del espacio de Datos */
		struct stat buffer; /* Estructura para info general de un archivo */

		if((fd = open(dataBin, O_RDONLY)) == -1)
		{
			log_error(logg, "Error al abir el archivo del espacio de datos del nodo: %s:%d, imposible obtener cantidad de bloques", nodoIP, nodoPort);
			exit(1);
		}
		else {
			/* Obtengo estadisticas del archivo */
			fstat(fd, &buffer);
			/* Obtengo la cantidad de bloques redondeando siempre para abajo ya que no puedo utilizar espacio que no tengo */
			blocks = floor(buffer.st_size / (1024 * 1024 * 20));
			close(fd);
		}

}

void initBlocksSem(){
	/* Necesito espacio para n semáforos (uno por cada bloque del espacio de datos) */
	semBloques = malloc(sizeof(pthread_rwlock_t) * blocks);
		int i;
		for (i = 0; i < blocks; i++) {
			if (pthread_rwlock_init(&semBloques[i], NULL) != 0) {
				log_error(logg, "Error al crear el mutex para los bloques.");
				exit(1);
			}
		}
}

void logConsoleOn(){
	/* Apaga los mensajes en la consola */
	logg->is_active_console = true;
}

void logConsoleOff(){
	/* Apaga los mensajes en la consola */
	logg->is_active_console = false;
}

int excec_file(char *stdin, char *script, char *stdout, int type) {

	int stdoutfd;  /* File Descriptor del stdout */
	char *comando; /* Comando como tal que se ejecutará por consola */
	int resultado; /* Resultado de aplicar la rutina */

	/* Creo el archivo donde guardaré el resultado de aplicar el script al stdin
	 * si esto no puediera realizarse devuelvo el control */
	stdoutfd = open(stdout, O_RDWR|O_CREAT,S_IRWXU);
	if(stdoutfd == -1) {
		log_error(logg, "Error al abir el archivo para ejecutar la rutina");
		return 0;
	}

	/* Armo el comando como lo necesito */
	if(type == 1){
		comando = string_duplicate(script);
		//Esto lo saco porque el reduce ya siempre hace sort
		//Esto serviría si hacemos el apareo por red, y no como lo hacemos
		//string_append(&comando," | sort > ");
		//Fue una decisión de diseño ya que la perfomance del sistema en general
		//aumento INFINITO.
		string_append(&comando," > ");
	} else {
		comando = string_duplicate("sort");
		string_append(&comando," | ");
		string_append(&comando, script);
		string_append(&comando, " > ");
	}

	string_append(&comando,stdout);

	/* La magia de los pipes */
	FILE *pipe = popen(comando, "w");

	if (!pipe) {
		log_error(logg, "Error al abir el pipe para ejecutar la rutina");
		return 0;
	}

	/* Mando el archivo al pipe donde se ejecutara el comando */
	resultado = fputs(stdin, pipe);

	if (resultado < 0) {
		log_error(logg, "Error al escribir sobre el pipe");
		pclose(pipe);
		return 0;
	}

	/* Cierro el pipe */
	pclose(pipe);
	/* Cierro el descriptor de archivo */
	close(stdoutfd);

	free(comando);

	return 1;

}

char* saveScriptFile(char* taskID, int type, char* script) {

	/* Puntero al archivo donde escribiré el script */
	FILE *fp;
	/*Nombre completo del script */
	char *scriptName;
	/* Nombre el script segun si es de mapping o reduce */
	if (type == 1) {
		scriptName = string_duplicate(dirScriptsMap);
	} else {
		scriptName = string_duplicate(dirScriptsRed);
	}
	string_append(&scriptName, taskID);
	/* Abro el archivo donde escribiré el script */
	fp = fopen(scriptName, "w+");
	/* Escribo los bytes del script en el archivo */
	fwrite(script, 1, strlen(script), fp);
	/* Cierro archivo */
	fclose(fp);
	/* Permisos para el script */
	char * comando = string_duplicate("chmod +rwx ");
	string_append(&comando, scriptName);
	system(comando);
	free(comando);
	/* Devuelvo el nombre completo del script */
	return scriptName;

}

char *getBloque(int blockNum) {


	pthread_rwlock_wrlock(&semBloques[blockNum]);
	/* Desactivo info por consola */
	logConsoleOff();
	/* Logueo la solicitud de devolución del contenido de un bloque */
	log_info(logg, "Recibida la solicitud para devolver el bloque N°: %d", blockNum);

	int fd = open(dataBin, O_RDWR); /* Obtengo el file descriptor del espacio de datos */
	char *buffer; /* Tendrá los datos de un bloque (aca hay converción de tipos) */

	/* Si el file descriptor == -1 hubo un error en el open */
	if (fd == -1) {
		log_error(logg, "El espacio de datos no pudo abrirse, imposible devolver el bloque N°: %d", blockNum);

		exit(1);
	}

	/* Al ejecutar el siguiente mmap buffer tiene la data de un bloque, tiene la
	 data, y no un puntero ya que buffer es de tipo char* */
	buffer = mmap((caddr_t) 0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd,
			BLOCK_SIZE * blockNum);

	/* si buffer no nos devuelve datos hubo un error en el mmap */
	if (buffer == (caddr_t) (-1)) {
		log_error(logg,"Error en el mmap, imposible devolver el bloque N°: %d", blockNum);

		exit(1);
	}

	/* Cierro el file descriptor */
	close(fd);
	/* Activo info por consola */
	logConsoleOn();

	pthread_rwlock_unlock(&semBloques[blockNum]);
/*
	FILE *f = fopen("file.txt", "w");
	fprintf(f, "Some text: %s\n", buffer);
	fclose(f);*/

	return buffer;

}

void setBloque(int blockNum,char *data) {

	pthread_rwlock_wrlock(&semBloques[blockNum]);
	/* Desactivo mensajes por consola */
	logConsoleOff();
	/* Logueo la solicitud de devolución del contenido de un bloque */
	log_info(logg, "Recibida la solicitud para cambiar el contenido del bloque N°: %d", blockNum);

	int dataLength = strlen(data) + 1;      /* Tamaño de la data a modificar + uno para que incluya el \0 */
	void *address;					  		/* Dirección de memoria en donde se mapeara el bloque solicitado en memoria*/

	/* Obtengo el file descriptor del espacio de datos */
	int fd = open(dataBin, O_RDWR);

	/* Si el file descriptor == -1 hubo un error en el open */
	if (fd == -1) {
		log_error(logg, "El espacio de datos no pudo abrirse, imposible escribir el bloque N°: %d", blockNum);

		exit(1);
	}

	/* Al ejecutar el siguiente mmap address tiene la dirección de memoria del bloque
	 donde debemos escribir el nuevo contenido */
	address = mmap((caddr_t) 0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd,
	BLOCK_SIZE * blockNum);

	/* si buffer no nos devuelve datos hubo un error en el mmap */
		if (address == (caddr_t) (-1)) {
			log_error(logg, "Error en el mmap, imposible escribir el bloque N°: %d", blockNum);

			exit(1);
		}

	/* Copio el bloque en el área de memoria compartida */
	memcpy(address, data, dataLength);
	/* Sincronizo memoria para asegurar que los cambios afecten al archivo */
	msync(address, dataLength, MS_SYNC);
	/* Libero la memoria. */
	munmap(address, BLOCK_SIZE);
	/* Cierro el file descriptor */
	close(fd);

	/* Activo mensajes por consola */
	logConsoleOn();
	pthread_rwlock_unlock(&semBloques[blockNum]);
}

void aparear(sys_blocks* estructura, char* archivo)
{
	/* Esta esctructura no puedes ser accedida por otros nodos mientras este haciendo un reduce */
	pthread_mutex_lock(&estructura->semApareo);
	int result = 0;
	/* Voy armando mi archivo final, que contiene todos los resultados de mappings */
	log_info(logg, "Agrego archivo para reduce local");
	string_append(&estructura->archivoFinal, archivo);
	estructura->countArchivos--;

	if(estructura->countArchivos==0)
	{
		log_info(logg, "Iniciando reduce por tarea #%s", estructura->taskID);

		/* Aplico reducción */
		result = excec_file(estructura->archivoFinal, estructura->scriptName, estructura->fileNameFinal,2);
		/* Aviso status */
		char* strIdNodo = string_itoa(idNodo);
		if (result == 1){
			runFunction(estructura->connection->socket, "nodo_endTask", 4, strIdNodo, "2", estructura->taskID, "1");
			log_info(logg, "Solicitud de reduce recibida por parte del Job: %s (IP: %s) terminada satisfactoriamente",estructura->taskID, estructura->connection->ip);
		}else{
			runFunction(estructura->connection->socket, "nodo_endTask", 4, strIdNodo, "2", estructura->taskID, "0");
			log_info(logg, "Solicitud de reduce recibida por parte del Job: %s (IP: %s) fallo",estructura->taskID, estructura->connection->ip);
		}
		free(strIdNodo);
		free(estructura->archivoFinal);
		free(estructura->taskID);
		free(estructura->scriptName);
		free(estructura->fileNameFinal);
		pthread_mutex_unlock(&estructura->semApareo);
		pthread_mutex_destroy(&estructura->semApareo);
		free(estructura);
	} else {
		pthread_mutex_unlock(&estructura->semApareo);
	}
}


// FUNCIONES DE COMUNICACIÓN

void fs_getInformation(socket_connection* socketConnection, char** args)
{
	char * idNodo_str = string_itoa(idNodo);
	char * blocks_str = string_itoa(blocks);
	char * nodoPort_str = string_itoa(nodoPort);
	runFunction(socketConnection->socket, "nodo_setInformation", 4, idNodo_str, blocks_str, nodoPort_str, nodoIP);
	free(idNodo_str);
	free(blocks_str);
	free(nodoPort_str);
}


void fs_existsNodo(socket_connection* socketConnection, char** args)
{
	log_info(logg, "Ya existe otro nodo con la misma ID.");
	exit(1);
}

void clientConnected(socket_connection* socketInfo)
{
	log_info(logg, "Se ha conectado un nuevo cliente con ip %s en socket n°%d", socketInfo->ip, socketInfo->socket);
}


void fs_connectionDropped(socket_connection * connection){

	/* Aviso que se perdió la conexión con el FS */
	log_info(logg, "Conexión con el FileSystem %s:%d  terminada", connection->ip,connection->port);
	exit(1);
}

void client_connectionDropped(socket_connection * connection){

	/* Aviso que se perdió la conexión con algun cliente */
	log_info(logg, "Conexión con el cliente %s:%d  terminada", connection->ip,connection->port);

}

void fs_getBloque(socket_connection * connection, char** args){

		int blockNum = atoi(args[0]); /* Número de bloque */
		char *buffer;				  /* Contenido del Bloque */

		buffer = getBloque(blockNum);
		runFunction(connection->socket, "nodo_setBloque", 1, buffer);

}

void fs_setBloque(socket_connection * connection, char** args) {

	int blockNum = atoi(args[0]);    		/* Número de bloque */

	setBloque(blockNum,args[1]);

	runFunction(connection->socket, "nodo_blockReady", 1, args[0]);

}

void fs_moveBlock(socket_connection* socketConnection, char** args){

	int fromBlock = atoi(args[0]); /* Bloque origen  */
	int toBlock   =	atoi(args[1]); /* Bloque destino  */
	char *block;				   /* Data del bloque */

	block = getBloque(fromBlock);
	setBloque(toBlock,block);
	runFunction(socketConnection->socket, "nodo_blockReady", 1, args[1]);
}

void getFileContent(socket_connection* socketConnection, char** args){


	//char *fileName = args[0]; /* Nombre del archivo */
	char *fileContent;		  /* Contenido del archivo */
	/* Nombre completo del path */
	char* fileName = string_duplicate("/tmp/");
	string_append(&fileName,args[0]);
	/* Obtengo el contenido del archivo */
	fileContent = getFileContentUtils(fileName);

	runFunction(socketConnection->socket, "setFileContent", 1, fileContent);

	free(fileContent);
}

void getFileResult(socket_connection * connection, char ** args)
{
	//char *fileName = args[0]; /* Nombre del archivo */
	char* fileName = string_duplicate("/tmp/");
	string_append(&fileName,args[0]);
	char *buffer = getFileContentUtils(fileName); /*Contenido del archivo */

	runFunction(connection->socket,"sendFileResult",1,buffer);

	free(buffer);
	free(fileName);
}

void sendFileResult(socket_connection* connection, char** args)
{
	char *buffer = args[0]; /* Contenido del archivo */
	sys_blocks *estructura = connection->data; /* Estructura del apareo */

	log_info(logg, "Llega info para apareo! de (%s:%d)", connection->ip, connection->port);
	/* Realizo el apareo correspondiente */
	aparear(estructura, buffer);

	/* Cierro la conexión */
	close(connection->socket);

}

void job_makeTask(socket_connection *connection, char** args)
{
	int i;
	int result;										/* Resultado de rutina */
	char *scriptName;                               /* Nombre completo del script */
	char *taskID = (args[0]);                       /* ID de tarea */
	int type = atoi(args[1]);                       /* Tipo de tarea (mapping o reduce) */
	char *script = args[2];	                        /* Contenido del script */
	//char *fileNameFinal = args[4];                  /* Nombre  del archivo resultante */
	//fileNameFinal = concat("/tmp/", fileNameFinal); /* Nombre completo */
	char* fileNameFinal = string_duplicate("/tmp/");
	string_append(&fileNameFinal,args[4]);

	/* Guardo el script en su correspondiente carpeta */
	scriptName = saveScriptFile(taskID, type, script);
	/* Bifurco la lógica para procesar mapping o reduce */
	if (type == 1) {
		//MAPPING
		/* Agrego log */
		log_info(logg, "Solicitud de mapping %s (IP: %s)", taskID, connection->ip);

		int blockNum = atoi(args[3]);
		char *contenidoDelBloque;
		/* Obtengo el contenido del bloque solicitado */
		contenidoDelBloque = getBloque(blockNum);
		/* Proceso el bloque solicitado con el script indicado */
		result = excec_file(contenidoDelBloque, scriptName, fileNameFinal, type);
		/* Informo resultado de la actividad de mapping */
		char* strIdNodo = string_itoa(idNodo);
		char* strType = string_itoa(type);
		if (result == 1){
			runFunction(connection->socket, "nodo_endTask", 4, strIdNodo, strType, taskID, "1");
			log_info(logg, "Rutina %s de mapping (IP: %s) OK", taskID, connection->ip);
		}else{
			runFunction(connection->socket, "nodo_endTask", 4, strIdNodo, strType, taskID, "0");
			log_info(logg, "Rutina %s de mapping (IP: %s) FAIL", taskID, connection->ip);
		}
		free(strIdNodo);
		free(strType);
		free(fileNameFinal);
		free(scriptName);
		munmap(contenidoDelBloque, BLOCK_SIZE);
		//REDUCE
	} else {
		/* Agrego log */
		log_info(logg, "Solicitud de reduce recibida por parte del Job: %s (IP: %s)", taskID, connection->ip);

		fns = dictionary_create(); /* Diccionario de funciones que mandare por socket a otro nodo */
		t_list* archivos = nodoFiles_deserialize(args[3]); /* Punteros a la info de un nodo específico */
		int countArchivos = list_size(archivos); /* Cantidad de punteros */
		nodoFile *nf; /* Puntero a la estructura contenedora de la info de un nodo */
		sys_blocks *myFilesToDo = malloc(sizeof(sys_blocks)); /* Estructura mágica para el apareo del reduce */
		int socket_nodo; /* Socket del nodo al que me conecto */

		/* Agrego la función que mandará el resultado de reduce entre nodos */
		dictionary_put(fns, "sendFileResult", &sendFileResult);

		/* Inicializo la estructura para el reduce */
		myFilesToDo->countArchivos = countArchivos;
		myFilesToDo->archivoFinal = string_new();
	    myFilesToDo->connection = connection;
		myFilesToDo->taskID = string_duplicate(taskID);
		myFilesToDo->scriptName = scriptName;
		myFilesToDo->fileNameFinal = fileNameFinal;
		pthread_mutex_init(&myFilesToDo->semApareo, NULL);

		/* Comienzo a procesar cada archivo indicado a ser reducido */
	   for(i=0;i<countArchivos;i++)	{

		   nf = list_get(archivos, i); /* Puntero a un archivo deserializado con la info de la data a reducir */

		   /* Si es mi nodo trabajo localmente */
			if(strcmp(nf->ip, nodoIP) == 0 && nf->port == nodoPort)
			{

			   //char *fileNameInicial = concat("/tmp/",nf->fileNameInicial);
				char* fileNameInicial = string_duplicate("/tmp/");
				string_append(&fileNameInicial,nf->fileNameInicial);
				char *contenidoArchivo = getFileContentUtils(fileNameInicial);

				aparear(myFilesToDo, contenidoArchivo);

				free(fileNameInicial);
				free(contenidoArchivo);
			}
			/* Si es otro nodo, me conecto por socket */
			else
			{
				if((socket_nodo = connectServer(nf->ip, nf->port, fns, NULL, myFilesToDo)) == -1)
				{
					runFunction(connection->socket, "nodo_endTask", 4, string_itoa(idNodo), string_itoa(type), taskID, "0");
					log_error(logg, "Error al intentar conectar con el nodo (%s,%d), abortando reduce", nf->ip, nf->port);
					break;
				} else
				{
					runFunction(socket_nodo, "getFileResult", 1, nf->fileNameInicial);
					log_info(logg, "Ejecutando reduce de forma remota con el nodo (%s,%d)", nf->ip, nf->port);
				}
			}
		}

	   //list_destroy_and_destroy_elements(archivos, (void*) free_nodoFile);
	}

}


int main(int argc, char *argv[]) {

	int socket_fs; /* Socket del FS */

	/* Leo archivo de configuracion */
	fileConfig = argc > 1 ? argv[1] : fileConfig; /*Cambio ruta de fileConfig si es pasado por argumento*/
	readFileConfig();

	/* Inicializo el log */
	logg = log_create(logPath, "Nodo", true, LOG_LEVEL_INFO);
	/* Obtengo la cantidad de bloques que componen el espacio de datos */
	getBlocksQuantity();
	/* Inicializo el semáforo para los bloques */
	initBlocksSem();
	/* Creo la estructura que me permite ejectuar funciones por socket */
	fns = dictionary_create();
	/* Agrego las funciones de comunicación que me solicitan ejecutar */
	dictionary_put(fns, "fs_getInformation", &fs_getInformation);
	dictionary_put(fns, "fs_existsNodo", &fs_existsNodo);
	dictionary_put(fns, "fs_getBloque", &fs_getBloque);
	dictionary_put(fns, "fs_setBloque", &fs_setBloque);
	dictionary_put(fns, "fs_moveBlock", &fs_moveBlock);
	dictionary_put(fns, "getFileContent", &getFileContent);
	dictionary_put(fns, "getFileResult", &getFileResult);
	dictionary_put(fns, "sendFileResult", &sendFileResult);
	dictionary_put(fns, "job_makeTask", &job_makeTask);

	/* Me conecto al FileSystem */
	if((socket_fs = connectServer(fsIP, fsPort, fns, &fs_connectionDropped, NULL) == -1)) {
		//Agrego Log
		log_error(logg, "Error al intentar conectarse al FileSystem(%s:%d).", fsIP, fsPort);

		exit(1);
	}else {
		log_info(logg, "Ready, nodo conectado con el FileSystem(%s:%d).", fsIP, fsPort);
	}

	/* Me mantengo a la escucha de Jobs y Nodos*/
	if (createListen(nodoPort, &clientConnected, fns, &client_connectionDropped, NULL) == -1){
		log_error(logg, "Error al crear un socket_server en el puerto: %d." ,nodoPort);

		exit(1);

	} else {
		log_info(logg, "A la escucha de Jobs y Nodos en el puerto: %d." ,nodoPort);
	}

	//Dejo bloqueado main
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	return 1;

}

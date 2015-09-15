#include <stdio.h>
#include <stdlib.h>
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
#include <socket.h>
#include <utils.h>

char* 				fileConfig = "config.cfg";	/* Ruta a archivo config por defecto */
int					id;							/* ID del job */
char* 				martaIP; 					/* Dirección IP de la computadora que está ejecutando el proceso MaRTA */
int 				martaPort; 					/* Puerto donde el proceso MaRTA está  escuchando nuevas conexiones. */
char* 				mapper; 					/* Ruta a programa rutina de mapping */
char* 				reduce; 					/* Ruta a programa rutina de reduce */
bool 				combiner; 					/* Determina si el Job soporta que se lo planifique en modo combiner */
char** 				files; 						/* Lista de archivos sobre los que se va a aplicar el job */
char* 				resultFile; 				/* Nombre del archivo donde se va a almacenar el resultado */
int					socket_marta;				/* Socket que apunta a marta */
t_dictionary * 		fns;						/* funciones de socket */
char * 				tmp;						/* char temporal para operaciones */
char 				spr[1024];					/* utilizada para los sprintf */
t_log*				logg;						/* Variable para logs de Job */

pthread_mutex_t mx_main;

//lee el archivo de configuración
void readFileConfig()
{
	t_config* config;
	config = config_create(fileConfig);

	if(config_has_property(config, "ID"))
		id = config_get_int_value(config, "ID");

	if(config_has_property(config, "IP_MARTA"))
		martaIP = string_duplicate(config_get_string_value(config, "IP_MARTA"));

	if(config_has_property(config, "PUERTO_MARTA"))
		martaPort = config_get_int_value(config, "PUERTO_MARTA");

	if(config_has_property(config, "MAPPER"))
		mapper = string_duplicate(config_get_string_value(config, "MAPPER"));

	if(config_has_property(config, "REDUCE"))
		reduce = string_duplicate(config_get_string_value(config, "REDUCE"));

	if(config_has_property(config, "COMBINER"))
		combiner = strcmp(config_get_string_value(config, "COMBINER"), "SI") == 0;

	if(config_has_property(config, "ARCHIVOS"))
		files = config_get_array_value(config, "ARCHIVOS");

	if(config_has_property(config, "RESULTADO"))
		resultFile = string_duplicate(config_get_string_value(config, "RESULTADO"));

	config_destroy(config);
}

//Devuelve nombre de tarea
char * getTaskStr(char * type)
{
	return type[0] == '1' ? "Mapping" : "Reduce";
}

//Funcion que se ejecuta cuando se desconecta marta
void connectionClosedMarta(socket_connection * connection)
{
	//Agrego Log
	log_error(logg, "Marta SOCK%d(%s:%d) se ha desconectado.", connection->socket, connection->ip, connection->port);

	//Finalizo
	exit(1);
}

//Funcion que se produce cuando se desconecta un nodo
void nodo_connectionClosed(socket_connection * connection)
{
	//todo: completar
}

//Marta indica que el ID ya se encuentra en uso por otro Job
void marta_jobID_repeat(socket_connection * connection, char ** args)
{
	//Agrego log
	log_error(logg, "El ID %d configurado ya se encuentra en uso por otro Job.", id);

	//Finalizamos proceso
	exit(1);
}

//Marta solicita aplicar tarea de Mapping o Reduce
void marta_makeTask(socket_connection * connection, char ** args)
{
	char * type = args[0];			/* Tipo de tarea a realizar (1=Mapping, 2=Reduce) */
	char * ip = args[1];			/* IP de nodo */
	int puerto = atoi(args[2]);		/* Puerto de nodo */
	char * taskID = args[3];		/* ID de la tarea a realizar */
	char * blocks = args[4];		/* Bloque/s a realizar tarea */
	char * fileName = args[5];		/* Nombre de archivo del espacio temporal */

	int socket_nodo;				/* socket de nodo */

	//me conecto a nodo
	socket_nodo = connectServer(ip, puerto, fns, &nodo_connectionClosed, NULL);

	//si se conecta le envio al nodo la tarea que tiene que hacer
	if(socket_nodo != -1)
	{
    	tmp = getFileContentUtils(type[0] == '1' ? mapper : reduce);
		runFunction(socket_nodo, "job_makeTask", 5, taskID, type, tmp, blocks, fileName);
		free(tmp);

		log_info(logg, "Creación de hilo %s, para tarea N°%s, bloques: %s, Archivo temporal: %s.", type, taskID, blocks, fileName);
	}
}

//Nodo me informa que finalizo tarea de mapping o reduce
void nodo_endTask(socket_connection * connection, char ** args)
{
	char * nodoID = args[0];		/* ID de nodo */
	char * type = args[1];			/* Tipo de tarea realizada (1=Mapping, 2=Reduce) */
	char * taskID = args[2];		/* ID de la tarea realizada */
	char * success = args[3];		/* Determina si tuvo o no exito la tarea (1=SI, 0=NO) */

	//Agrego log
	log_info(logg, "Finalización de hilo %s, de tarea N°%s, Resultado: %s.", type, taskID, success ? "Exitoso" : "No exitoso");

	//Informo a marta lo sucedido
	runFunction(socket_marta, "job_succesTask", 4, type, nodoID, taskID, success);

	//finalizo conexion con nodo
	connection->run_fn_connectionClosed = false;
	close(connection->socket);
}

//marta indica que finalizo el Job
void marta_end(socket_connection * connection, char ** args)
{
	char * result = args[0];	/* Resultado final el cual se guarda en result.txt */

	if(atoi(result)){
		log_info(logg, "Finalización del job de manera exitosa!");
	} else {
		log_info(logg, "Finalización del job con errores: %s.", args[1]);
	}

	pthread_mutex_unlock(&mx_main); //desbloqueo el main para que finalize
}


int main(int argc, char *argv[])
{
	//Inicializo LOG
	logg = log_create("job.log", "Job", true, LOG_LEVEL_INFO);

	//leo archivo de configuracion
	fileConfig = argc > 1 ? argv[1] : fileConfig; /*Cambio ruta de fileConfig si es pasado por argumento*/
	readFileConfig();

	//Diccionario de funciones de comunicacion
	fns = dictionary_create();
	dictionary_put(fns, "marta_makeTask", &marta_makeTask);
	dictionary_put(fns, "nodo_endTask", &nodo_endTask);
	dictionary_put(fns, "marta_end", &marta_end);

	//Me conecto a marta, si hay error informo y finalizo
	if((socket_marta = connectServer(martaIP, martaPort, fns, &connectionClosedMarta, NULL)) == -1)
	{
		//Agrego Log
		log_error(logg, "Error al conectar a marta(%s:%d).", martaIP, martaPort);

		exit(1);
	}

	//Agrego Log
	log_info(logg, "Se ha conectado exitosamente a marta en ip %s en puerto %d en socket n°%d.", martaIP, martaPort, socket_marta);

	//le digo a marta que comience un job
	tmp = implode(files, ",");
	runFunction(socket_marta, "job_start", 4, string_itoa(id), tmp, combiner ? "1" : "0", resultFile);
	free(tmp);

	//Dejo bloqueado main
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	//Finalizo LOG
	log_destroy(logg);

	return EXIT_SUCCESS;
}

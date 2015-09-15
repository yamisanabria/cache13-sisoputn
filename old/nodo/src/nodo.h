/* Firmas de mis funciones */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
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
#include <socket.h>

// STRUCTS

/* Estructura que se utiliza para realizar las rutinas de reduce */
typedef struct sys_blocks
	 {
	     int countArchivos ;
		 char *archivoFinal;
		 socket_connection * connection;
		 char* taskID;
		 char *scriptName;
		 char *fileNameFinal;
		 pthread_mutex_t semApareo;
	} sys_blocks;



//FUNCIONES LOCALES

/* Mapea un archivo de configuración e inicializa las variables globales del sistema */
void readFileConfig();

/* Actualiza la variable global "blocks" con la cantidad de bloques que dispone el nodo */
void getBlocksQuantity();

/* Inicializa los semáforos que se utilizarán para manejar la concurrencia al acceder a los bloques */
void initBlocksSem();

/* Activa mensajes de log por consola */
void logConsoleOn();

/* Desactiva mensajes de log por consola */
void logConsoleOn();

/* Ejecuta un comando por consola. La ruta del programa principal es: *script, la entrada de
 * ese programa es el conjunto de bytes (*stdin), el resultado se escribe en la ruta (*stdout),
 *  el comando por consola varía segun el valor de type (mapping o reduce).
 */
int excec_file(char *stdin, char *script, char *stdout, int type);

/* Guarda el contenido de un script (char *script) en el directorio del nodo "mapping" o "reduce",
 * segun el valor de  type, con el nombre: taskID
 */
char* saveScriptFile(char* taskID, int type, char* script);

/* La información del proceso que lo llama se encuentra en connection. Devuelve el contenido del
 * bloque cuyo valor esta almacenado en el argumento args[0];
 */

/* Retorna el contendido del bloque número "numeroDeBloque" */
char* getBloque(int numeroDeBloque);

/* Escribe el bloque número "blockNum", con el contenido que se encuentra en *data */
void setBloque(int blockNum, char *data);

/* Recibe una estructura que acumula los archivos mapeados del nodo local u otros nodos remotos
 * (*archivo). Cuando no hay mas archivos para agregar, a la estructura->archivoFinal
 *  se aplica el reduce.
 */
void aparear(sys_blocks* estructura, char* archivo);

/* La información del proceso que lo llama se encuentra en connection. Escribe el número de bloque
 * args[0] con la data enviada en args[1]  */

// FUNCIONES DE COMUNICACIÓN

/* Recibe la información del FS en socketConnection. El resultado de ejecutar esta función
 * es comunicarle al FS la cantidad de bloques que dispone el nodo, el puerto sobre el que
 * esta conectado y el identificador único de nodo.
 */
void fs_getInformation(socket_connection* socketConnection, char** args);

/* Si FS llama a ejecutar esta función, termina con el proceso nodo ya que el mismo ya existe,
 * es decir, bloquea una segunda conexión.
 */
void fs_existsNodo(socket_connection* socketConnection, char** args);

/* Cada vez que alguien se conecta a nosotros, tenemos la info del cliente socketInfo */
void clientConnected(socket_connection* socketInfo);

/* Informa que se perdió la conexión con el FS "*connection" */
void fs_connectionDropped(socket_connection * connection);

/* Informa que se cayó la conexión con algun cliente */
void client_connectionDropped(socket_connection * connection);

/* Llama a la función local getBloque pasandole args[0] (número de bloque) como argumento y
 * retorna el resultado a la conexión (connection) llamante.
 */
void fs_getBloque(socket_connection * connection, char** args);

/* Llama a la función local setBloque pasandole args[0] (número de bloque), args[1] (contenido
 * del bloque, como argumentos y retorna el resultado a la conexión (connection) llamante.
 */
void fs_setBloque(socket_connection * connection, char** args);

/* Mueve el contenido del bloque número: args[0] al bloque número args[1] e informa el resultado */
void fs_moveBlock(socket_connection* socketConnection, char** args);

/* Llama a la función de la librería de utilidades getFileContentUtils. Esta función retorna al
 * proceso llamante el contenido del archivo con el nombre en args[0]
 */
void getFileContent(socket_connection* socketConnection, char** args);

/* Función para el reduce. Un nodo solicita a otro nodo que le de el contenido del archivo cuyo
 * nombre es args[0] y se lo envía de nuevo al nodo llamante invocando sendFileResult()
 */
void getFileResult(socket_connection * connection, char ** args);

/* Función para el reduce. Almacena el resultado obtenido en getFileResult y llama a la función
 * apareo()
 */
void setFileResult(socket_connection * connection, char ** args);

/* Esta es LA función del nodo, básicamente se encarga de resolver un job (MAP o REDUCE)
 *
 *args[0] = ID de tarea realizada
 *args[1] = Tipo de tarea a realizar (1=Mapping, 2=Reduce)
 *args[2] = archivo de mapper o reduce
 *args[3] = bloque/s a realizar tarea (si type=1 blocks es un número de bloque,
 *si type=2 blocks es el conjunto de bloques que ademas indica en que nodos estan)
 *args[4] = nombre de archivo del espacio temporal

 *block con type=2
 *IP, Port, fileNameIncial
 *IP=IP de nodo
 *Port=Puerto de nodo
 *fileNameInicial=nombre del archivo mapping que tiene el nodo */
 void job_makeTask(socket_connection *connection, char** args);







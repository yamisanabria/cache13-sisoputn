/*
 * swap.h
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

// STRUCTS

/* Estructura para adminsitrar el espacio utilizado */
typedef struct list_proc {
	int pid;
	int offset;
	int pageQty;
	int reads;
	int writes;
} list_proc;

/* Estructura para adminsitrar el espacio libre */
//typedef struct list_holes
//	 {
//	     int offset;
//		 int pageQty;
//	} list_holes;
//DEFINICIONES DE LAS FUNCIONES PROPIAS
/* Función que lee el archivo de configuración */
void readFileConfig();

/* Función que inicializa el log */
void initLog();

/* Función que creará el swap.data con su configuración correspondiente */
void initSwap();

/* Función inicializa la lita para administrar el espacio utilizado y el libre */
void initList();

/* Función que dado un pid, una posición inicial y una cantidad de páginas,
 * retorna un puntero a la estructura que representa un nuevo nodo en la lista
 */
list_proc *createNode(int pid, int pos, int pages);

/* Función que retorna el contenido leido de una página(una porción del SWAP) */
char *readSwap(int procStart, int pages);

/* Función que escribe una determinada cantidad de bytes en el SWAP */
void writeSwap(int procStart, int pages, char *data);

/* Función que compacta los procesos. Esto implica acomodar los nodos de la lista,
 * y acomodar los procesos en SWAP.
 */
int compact();

/* Función que asignará tamaño necesario para que el proceso cuya id "pid"
 * sea guardado. Esto implica crear un nodo nuevo en la lista y quizá compactar */
int initProc(int pid, int requiredPages);

/* Función que de ser posible llamara a initProc para iniciazliar un proceso.
 * En caso de no poder le informará al adm. de memoria que no hay espacio disponible.
 */
void startProcess(socket_connection *conn, int pid, int pageQty);

/* Función que terminará y liberará el proceso con id: pid */
void endProcess(socket_connection *conn, int pid);

/* Función que leerá la pagina requerida y le devolverá el contenido al proceso adm. de mem. */
void pageReadRequest(socket_connection *conn, int pid, int page);

/* Función que escribirá el contenido solicitado en la página requerida */
void pageWriteRequest(socket_connection *conn, int pid, int pageNum, char* data);


#endif /* SWAP_H_ */

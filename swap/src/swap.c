/*
 ============================================================================
 Name        : swap.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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
#include <string.h>
#include <pthread.h>
#include <socket.h>
#include "connections.h"
#include "shared.h"
#include "swap.h"
#include "utils.h"
#include "tests.h"

//VARIABLES GLOABLES

char *fileConfig = "config.cfg"; /* Ruta del archivo de configuración */
char *swapName; /* Nombre del archivo swap */
int pageQuan; /* Cantidad de páginas */
int pageSize; /* Tamaño de la página en bytes */
int t_swap; /* Tiempo del retardo de intercambio (swap-in o swap-out) */
int t_compaction; /* Tiempo del retardo de compactación */
int pageDisp; /* Páginas disponibles en el swap */

FILE *swap;  /* Puntero al archivo de swap */
t_log *logg; /* Variable para el log (tambien definida como extern en "shared.h) */
t_list *lp;  /* Puntero a la lista para administrar el espacio utilizado */
pthread_mutex_t mx_main; /* Mutex para el proceso main */

//FUNCIONES PROPIAS

void readFileConfig() {

	/* Si la ruta del archivo de configuración, pasada por argumento
	 * es distinta que la preconfigurada, la cambio..  */

	t_config* config;
	config = config_create(fileConfig);

	setSwapPort(config);

	if (config_has_property(config, "NOMBRE_SWAP"))
		swapName = string_duplicate(
				config_get_string_value(config, "NOMBRE_SWAP"));

	if (config_has_property(config, "CANTIDAD_PAGINAS"))
		pageQuan = config_get_int_value(config, "CANTIDAD_PAGINAS");

	if (config_has_property(config, "TAMANIO_PAGINA"))
		pageSize = config_get_int_value(config, "TAMANIO_PAGINA");

	if (config_has_property(config, "RETARDO_SWAP"))
		t_swap = config_get_int_value(config, "RETARDO_SWAP");

	if (config_has_property(config, "RETARDO_COMPACTACION"))
		t_compaction = config_get_int_value(config, "RETARDO_COMPACTACION");

	config_destroy(config);

}

void initLog() {

	logg = log_create("swap.log", "Swap", true, LOG_LEVEL_INFO);
}

void initSwap() {

	char *command = string_duplicate("dd if=/dev/zero of="); /* Prefijo del commando dd */

	/* Me armo el comando completo según los parámetros configurados */
	string_append(&command, swapName);
	string_append(&command, " bs=");
	string_append(&command, string_itoa(pageSize));
	string_append(&command, " count=");
	string_append(&command, string_itoa(pageQuan));

	system(command); /* Ejecuto el comando */
	swap = fopen(swapName, "rb+"); /* Abro el archivo para lectura y escritura */
	fseek(swap, 0, SEEK_SET); /* Pongo el puntero al comienzo del archivo */
	pageDisp = pageQuan; /* Inicialmente tengo todas las páginas disponibles */
	initList(); /* Inicializo la lista */

	log_info(logg, "El archivo %s, de %d bytes, fue creado exitosamente",
			swapName, pageSize * pageQuan);

	free(command);
}

void initList() {

	lp = list_create();

}

list_proc *createNode(int pid, int pos, int pages) {

	list_proc *nodo = malloc(sizeof(list_proc));
	nodo->pid = pid;
	nodo->offset = pos;
	nodo->pageQty = pages;
	nodo->reads = 0;
	nodo->writes = 0;

	return nodo;
}

void setZeros(int beginPos, int pages) {

	int size = pages * pageSize;
	/* lleno mi array de \0 para "eliminar" mi proceso del swap */
	char *zeros = calloc(1, pages * size + 1);
	/* también podía hacerse así
	 * fseek(swap, beginPos * pageSize, SEEK_SET);
	 * zeros = malloc(size);
	 * memset(zeros, '\0', size);
	 * fwrite(zeros, 1, size, swap);
	 */
	fseek(swap, beginPos * pageSize, SEEK_SET);
	fwrite(zeros, 1, size, swap);
	fflush(swap);
	free(zeros);
}

char *readSwap(int procStart) {

	int offset = procStart * pageSize;    //desde donde comienzo a leer
	char *process = calloc(1, pageSize + 1); //se necesita +1 para el '\0' de mierda

	fseek(swap, offset, SEEK_SET);
	fread(process, pageSize, 1, swap);

	return process;

}

void writeSwap(int procStart, char *data, int size) {

	int offset = procStart * pageSize; //desde donde comienzo a escribir
	int zeros = pageSize - size;

	fseek(swap, offset, SEEK_SET); // me posiciono en donde comenzaré a escribir

	if (zeros != 0) {
		char *s = calloc(1,pageSize+1);	//lleno un array dinámico con '\0'
		memcpy(s,data,size);			//copio los n bytes de ese array con la palabra a escribir donde n es la longitud de palabra
		fwrite(s, 1, pageSize, swap);
		free(s);
	} else {
		fwrite(data, pageSize, 1, swap);

	}

	fflush(swap);


}

int compact() {

	log_info(logg, "Compactación iniciada por fragmentación externa");

	int beginPos = 0; /* Inicio del siguiente mProc */
	char *process;    /* Contenido del mProc */

	void _set(list_proc *l) {
		int i; /* Página actual */
		int swapReadPos = l->offset; /* La posición de la que debo leer en swap */
		int swapWritePos = beginPos; /* La posición de la que debo leer en swap */
		l->offset = beginPos;        /* Asigno nuevo comienzo del mProc */

		for (i = 0; i < l->pageQty; i++) {
			/* Leo la información de la página actual del mProc en swap */
			process = readSwap(swapReadPos);
			/* Escribo en swap, el contenido en su nueva ubicación */
			writeSwap(swapWritePos, process, strlen(process));
			swapReadPos++;
			swapWritePos++;
		}

		/* Preparo la nueva ubicación para el siguiente proceso */
		beginPos += l->pageQty;
	}

	/* Itero la lista para compactar esos mProc */
	list_iterate(lp, (void*) _set);

	/* Pongo a dormir el proceso simulando el tiempo de retardo de compactación */
	usleep(t_compaction);

	log_info(logg, "Compactación finalizada");

	return beginPos;
}

int initProc(int pid, int requiredPages) {

	int beginPos = 0;  // Posición inicial(offset del proc)
	int acum = 0;	   // Huecos libres acumulados
	int dif = 0; 	  // Dif entre el lugar donde termina un proc y empieza otro
	int index = 0;     // Índice de la lista donde se insertará un nuevo nodo
	list_proc *nodo;

	/* inner function */
	bool _getInit(list_proc *l) {

		dif = l->offset - beginPos;

		if (dif >= requiredPages) {
			//Nodo, encotrado. Insertar
			nodo = createNode(pid, beginPos, requiredPages);
			list_add_in_index(lp, index, nodo);
			return 1;

		}
		// Establezco el nuevo comienzo
		beginPos = l->offset + l->pageQty;
		// Acumulo los huecos disponibles
		acum += dif;
		// Índice actual de la lista
		index++;
		return 0;

	}/* end of inner function */

	/* Si la lista esta vacía inserto directamente el nuevo nodo */
	if (list_is_empty(lp)) {
		nodo = createNode(pid, 0, requiredPages);
		list_add(lp, nodo);
		beginPos = 0;
	} else { /* Sino busco un hueco donde quepa */
		if (!list_find(lp, (void*) _getInit)) { /* Sino hay huecos, pero se deduce que entra al final lo inserto al final */
			if (pageDisp - acum >= requiredPages) {
				nodo = createNode(pid, beginPos, requiredPages);
				list_add(lp, nodo);
			} else { /* Sino compacto el espacio de swap e inserto al final*/
				beginPos = compact();
				nodo = createNode(pid, beginPos, requiredPages);
				list_add(lp, nodo);
			}
		}
	}

	/* Relleno con '\0' el espacio que utilizará este mProc en swap */
	setZeros(beginPos, requiredPages);

	/* Actualizo cantidad de páginas disponibles */
	pageDisp -= requiredPages;
	return beginPos;
}

void startProcess(socket_connection *conn, int pid, int requiredPages) {

	write_start();

	int initialByte;
	char *procID = string_itoa(pid);

	if (pageDisp >= requiredPages) {
		initialByte = initProc(pid, requiredPages);
		log_info(logg,
				"Proceso asignado exitosamente.. PID: %d, N° de byte inicial"
						" %d, Tamaño: %d bytes", pid, initialByte * pageSize,
				requiredPages * pageSize);

		sw_mem_startProcessOk(conn, procID);
	} else {
		log_info(logg,
				"Proceso rechazado. Espacio en swap insufuciente.. PID: %d",
				pid);
		sw_mem_noSpace(conn, procID);
	}

	free(procID);

	write_end();

}

void endProcess(socket_connection *conn, int pid) {

	int index = 0;
	int size = 0;
	int pages = 1;
	int procStart = 0;
	int rp = 0;
	int wp = 0;

	bool _getEnd(list_proc *l) {
		if (l->pid == pid) {
			procStart = l->offset;
			pages = l->pageQty;
			rp = l->reads;
			wp = l->writes;
			return 1;
		} else {
			index++;
			return 0;
		}
	}

	list_find(lp, (void*) _getEnd);
	list_remove(lp, index);

	pageDisp += pages;

	log_info(logg, "Proceso liberado exitosamente.. PID: %d, N° de byte inicial"
			" %d, Tamaño: %d bytes, Páginas leídas: %d, Páginas escritas: %d",
			pid, procStart * pageSize, size, rp, wp);
}

void pageReadRequest(socket_connection *conn, int pid, int pageNum) {

	read_start();

	usleep(t_swap); /*Retardo de swap*/

	int procStart = 0;
	char *id = string_itoa(pid);
	char *pn = string_itoa(pageNum);
	char *data;

	bool _getPage(list_proc *l) {
		if (l->pid == pid) {
			procStart = l->offset + pageNum;
			l->reads++;
			return 1;
		} else {
			return 0;
		}
	}

	list_find(lp, (void*) _getPage);

	data = readSwap(procStart);

	log_info(logg, "Lectura solicitada.. PID: %d, N° de byte inicial"
			" %d, Tamaño: %d bytes, Contenido: %s", pid, procStart * pageSize,
			strlen(data), data);

	sw_mem_page(conn, id, pn, data);

	free(id);
	free(pn);
	free(data);

	read_end();
}

void pageWriteRequest(socket_connection *conn, int pid, int pageNum, char* data) {

	write_start();

	usleep(t_swap); /*Retardo de swap*/

	int procStart = 0;

	bool _setPage(list_proc *l) {
		if (l->pid == pid) {
			procStart = l->offset + pageNum;
			l->writes++;
			return 1;
		} else {
			return 0;
		}
	}

	list_find(lp, (void*) _setPage);
	writeSwap(procStart, data, strlen(data));

	log_info(logg, "Escritura solicitada.. PID: %d, N° de byte inicial"
			" %d, Tamaño: %d bytes, Contenido: %s", pid, procStart * pageSize,
			strlen(data), data);

	write_end();
}

int main(int argc, char *argv[]) {

	readFileConfig(); /* Leo archivo de configuracion */
	initLog(); /* Inicializo el log */
	initSwap(); /* Creo el archivo de swap */
	initSyn(); /* Inicializo semáforos */
	initDictionary(); /* Inicializo el diccionario de funciones remotas */
	startListener(); /* Me pongo a la escucha de conexiones provenientes del Adm. de Memoria */

	//TEST
	//startTesting();
	//FIN DE ESE CASO DE USO

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	log_destroy(logg);

	return EXIT_SUCCESS;
}

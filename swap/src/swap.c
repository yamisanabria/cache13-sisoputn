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

//VARIABLES GLOABLES

char *fileConfig = "config.cfg"; /* Ruta del archivo de configuración */
char *swapName; /* Nombre del archivo swap */
int pageQuan; /* Cantidad de páginas */
int pageSize; /* Tamaño de la página */
int t_compaction; /* Tiempo del retardo de compactación */
int pageDisp; /* Páginas disponibles en el swap */

FILE *swap; /* Puntero al archivo de swap */
t_log *logg; /* Variable para el log (tambien definida como extern en "shared.h) */
t_list *lp; /* Puntero a la lista para administrar el espacio utilizado */
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
	swap = fopen(swapName, "r+"); /* Abro el archivo para lectura y escritura */
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

	return nodo;
}

int compact() {

	log_info(logg, "Compactación iniciada por fragmentación externa");

	int beginPos = 0;
	int index = 0;

	void _set(list_proc *l) {

		switch (index) {
		case 0:
			if (l->offset == 0){
				beginPos = l->pageQty;
				index++;
			}else{
				 l->offset = 0;
				 beginPos = l->pageQty;
				 index++;
			}
			break;
		default:
			l->offset = beginPos;
			beginPos += l->pageQty;
		}

	}

	list_iterate(lp, (void*) _set);

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
	bool _get(list_proc *l) {

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
		acum = +dif;
		// Índice actual de la lista
		index++;
		return 0;

	}

	/* Si la lista esta vacía inserto directamente el nuevo nodo */
	if (list_is_empty(lp)) {
		nodo = createNode(pid, 0, requiredPages);
		list_add(lp, nodo);
		beginPos = 0;
	} else {
		if (!list_find(lp, (void*) _get)) {
			if (pageDisp - acum >= requiredPages) {
				nodo = createNode(pid, beginPos, requiredPages);
				list_add(lp, nodo);
			} else {
				int bPos;
				bPos = compact();
				nodo = createNode(pid, bPos, requiredPages);
				list_add(lp, nodo);
			}
		}
	}

	pageDisp -= requiredPages;
	return beginPos;
}

void startProcess(socket_connection *conn, int pid, int requiredPages) {

	int initialByte;

	if (pageDisp >= requiredPages) {
		initialByte = initProc(pid, requiredPages);
		log_info(logg,
				"Proceso asignado exitosamente.. PID: %d, N° de byte inicial"
						" %d, Tamaño: %d bytes", pid, initialByte,
				requiredPages);
	} else {
		log_info(logg,
				"Proceso rechazado. Espacio en swap insufuciente.. PID: %d",
				pid);
		runFunction(conn->socket, "sw_mem_noSpace(id) ", 1, pid);
	}

	/* Si hay un hueco disponible, lo agrego a la lista de espacio utilizado */

}

void readSwap(int pid) {

}

void writeSwap(int procStart, int procSize) {

}

void freeProc(int pid) {

}

int main(int argc, char *argv[]) {

	readFileConfig(); /* Leo archivo de configuracion */
	initLog(); /* Inicializo el log */
	initSwap(); /* Creo el archivo de swap */
	initDictionary(); /* Inicializo el diccionario de funciones remotas */
	startListener(); /* Me pongo a la escucha de conexiones provenientes del Adm. de Memoria */

	//DEBUG
	// BUSCAR ESPACIO E INSERTAR EN LA LISTA
	/*socket_connection *conn = malloc(sizeof(socket_connection));
	 list_proc *nodo1 = malloc(sizeof(list_proc));
	 list_proc *nodo2 = malloc(sizeof(list_proc));
	 nodo1->pid = 1;
	 nodo1->offset = 0;
	 nodo1->pageQty = 5;
	 list_add_in_index(lp,0, nodo1);
	 nodo2->pid = 2;
	 nodo2->offset = 7;
	 nodo2->pageQty = 2;
	 list_add(lp, nodo2);
	 pageDisp = 2;
	 startProcess(conn, 3, 2);*/
	//FIN DE ESE CASO DE USO
	// BUSCAR, COMPACTAR E INSERTAR
	socket_connection *conn = malloc(sizeof(socket_connection));
	list_proc *nodo1 = malloc(sizeof(list_proc));
	list_proc *nodo2 = malloc(sizeof(list_proc));
	nodo1->pid = 1;
	nodo1->offset = 0;
	nodo1->pageQty = 4;
	list_add_in_index(lp, 0, nodo1);
	nodo2->pid = 2;
	nodo2->offset = 6;
	nodo2->pageQty = 2;
	list_add(lp, nodo2);
	pageDisp = 3;
	startProcess(conn, 3, 3);
	//FIN DE ESE CASO DE USO

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	log_destroy(logg);

	return EXIT_SUCCESS;
}

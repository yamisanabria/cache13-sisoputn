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

char *fileConfig = "config.cfg";  /* Ruta del archivo de configuración */
char *swapName;					  /* Nombre del archivo swap */
int   pageQuan;				      /* Cantidad de páginas */
int   pageSize;				      /* Tamaño de la página */
int   t_compaction;               /* Tiempo del retardo de compactación */
int   pageDisp;                   /* Páginas disponibles en el swap */

FILE *swap;                       /* Puntero al archivo de swap */
t_log *logg;					  /* Variable para el log (tambien definida como extern en "shared.h) */
t_list *lp;						  /* Puntero a la lista para administrar el espacio utilizado */
pthread_mutex_t mx_main;          /* Mutex para el proceso main */

//FUNCIONES PROPIAS

void readFileConfig() {

	/* Si la ruta del archivo de configuración, pasada por argumento
	 * es distinta que la preconfigurada, la cambio..  */

	t_config* config;
	config = config_create(fileConfig);

	setSwapPort(config);

	if (config_has_property(config, "NOMBRE_SWAP"))
		swapName = string_duplicate(config_get_string_value(config, "NOMBRE_SWAP"));

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

	char *command = string_duplicate("dd if=/dev/zero of=");    /* Prefijo del commando dd */

	/* Me armo el comando completo según los parámetros configurados */
	string_append(&command, swapName);
	string_append(&command, " bs=");
	string_append(&command, string_itoa(pageSize));
	string_append(&command, " count=");
	string_append(&command, string_itoa(pageQuan));

	system(command);	          /* Ejecuto el comando */
	swap = fopen(swapName,"r+");  /* Abro el archivo para lectura y escritura */
	fseek(swap, 0, SEEK_SET);	  /* Pongo el puntero al comienzo del archivo */
	pageDisp = pageQuan;          /* Inicialmente tengo todas las páginas disponibles */
	initList();				      /* Inicializo la lista */

	log_info(logg, "El archivo %s, de %d bytes, fue creado exitosamente", swapName, pageSize*pageQuan);

	free(command);
}

void initList(){

	lp = list_create();

}

int initProc(int pid, int requiredPages){

	int beginPos = 0;
	int found = 0;
	int temp;

	void _get(list_proc *l, int requiredPages){

		temp = l->offset - beginPos;

		if(temp>=requiredPages){
			//aca tengo mi espacio disponible
			found = 1;
		}

		beginPos = l->offset + l->pageQty;

		if(found || pageDisp - beginPos >= requiredPages)
		{
			//crear nodo
		}


	}

	/* Si la lista esta vacía inserto directamente el nuevo nodo */
	if(list_is_empty(lp)){
		list_proc nodo = malloc(sizeof(list_proc));

	}else{
	/*Sino, debo iterar la lista y ver donde se encuentra el espacio disponible */

	}
	list_iterate(lp, (void*)_get);


}


void startProcess(int pid, int requiredPages){

	if(pageDisp>=requiredPages){
		initProc(pid, requiredPages);
	}

	/* Si hay un hueco disponible, lo agrego a la lista de espacio utilizado */


}

void writeSwap(int procStart, int procSize){

}


int main(int argc, char *argv[]) {

	readFileConfig();                             /* Leo archivo de configuracion */
	initLog();                                    /* Inicializo el log */
	initSwap();									  /* Creo el archivo de swap */
	initDictionary();                             /* Inicializo el diccionario de funciones remotas */
	startListener();						      /* Me pongo a la escucha de conexiones provenientes del Adm. de Memoria */
	startProcess(1,2);
	startProcess(2,3);
	startProcess(3,3);
	startProcess(4,1);

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_destroy(&mx_main);

	log_destroy(logg);

	return EXIT_SUCCESS;
}

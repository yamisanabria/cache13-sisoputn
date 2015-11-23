/*
 * connections.c
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <socket.h>

#include "shared.h"
#include "swap.h"

//VARIABLES GLOBALES

int swapPort; /* Puerto del Administrador de Memoria */

t_dictionary *fns; /* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/

//FUNCIONES DE COMUNICACIÓN

/* Llamadas por otro proceso */

void mem_sw_startProcess(socket_connection *conn, char **args) {

	int pid = atoi(args[0]);
	int pq = atoi(args[1]);

	startProcess(conn, pid, pq);
}

void mem_sw_getPage(socket_connection *conn, char **args) {

	int pid = atoi(args[0]);
	int pag = atoi(args[1]);

	pageReadRequest(conn, pid, pag);
}

void mem_sw_setPage(socket_connection *conn, char **args) {

	int pid = atoi(args[0]);
	int pag = atoi(args[1]);
	char *data = string_duplicate(args[2]);

	pageWriteRequest(conn, pid, pag, data);

}

void mem_sw_endProcess(socket_connection *conn, char **args) {

	int pid = atoi(args[0]);

	endProcess(conn, pid);

}

void mem_sw_swapping(socket_connection *conn, char **args) {

	int pid = atoi(args[0]);
	int setPage = atoi(args[1]);
	char *data = string_duplicate(args[2]);
	int getPage = atoi(args[3]);

	pageWriteRequest(conn, pid, setPage, data);
	pageReadRequest(conn, pid, getPage);

}

/* Llamadas por mí */

void sw_mem_startProcessOk(socket_connection *conn, char *pid) {

	runFunction(conn->socket, "sw_mem_startProcessOk", 1, pid);
}

void sw_mem_noSpace(socket_connection *conn, char *pid) {

	runFunction(conn->socket, "sw_mem_noSpace", 1, pid);
}

void sw_mem_page(socket_connection *conn, char *pid, char *page, char *data) {

	runFunction(conn->socket, "sw_mem_page", 3, pid, page, data);
}

//FUNCIONES DE CONEXIÓN

void setSwapPort(t_config *config) {

	if (config_has_property(config, "PUERTO_ESCUCHA"))
		swapPort = config_get_int_value(config, "PUERTO_ESCUCHA");
}

void initDictionary() {
	/* Acá se agregarán todas las funciones que llaman en mi código */
	fns = dictionary_create();
	/* Agrego las funciónes que serán llamadas por otros procesos a mi diccionario */
	dictionary_put(fns, "mem_sw_startProcess", &mem_sw_startProcess);
	dictionary_put(fns, "mem_sw_getPage", &mem_sw_getPage);
	dictionary_put(fns, "mem_sw_setPage", &mem_sw_setPage);
	dictionary_put(fns, "mem_sw_endProcess", &mem_sw_endProcess);
	dictionary_put(fns, "mem_sw_swapping", &mem_sw_swapping);

}

void memAdmConnected(socket_connection* conn) {

	log_info(logg,
			"Se ha conectado un proceso Adm. de Memoria con ip %s en socket n°%d",
			conn->ip, conn->socket);
}

void memAdmDisconnected(socket_connection * conn) {

	log_info(logg, "Conexión con el Adm. de Memoria %s:%d  terminada", conn->ip,
			conn->port);
}

void startListener() {

	if (createListen(swapPort, &memAdmConnected, fns, &memAdmDisconnected, NULL)
			== -1) {
		log_error(logg, "Error al crear un socket_server en el puerto: %d.",
				swapPort);
		exit(1);

	} else {
		log_info(logg,
				"A la escucha de algún proceso Administrador de Memoria el puerto: %d.",
				swapPort);
	}
}


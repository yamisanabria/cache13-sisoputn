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

int swapPort;      /* Puerto del Administrador de Memoria */

t_dictionary *fns; /* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/


//FUNCIONES DE CONEXIÓN

void setSwapPort(t_config *config) {

	if (config_has_property(config, "PUERTO_ESCUCHA"))
		swapPort = config_get_int_value(config, "PUERTO_ESCUCHA");
}

void initDictionary() {
	/* Acá se agregarán todas las funciones que llaman en mi código */
	fns = dictionary_create();

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

	if (createListen(swapPort, &memAdmConnected, fns, &memAdmDisconnected, NULL) == -1) {
		log_error(logg, "Error al crear un socket_server en el puerto: %d.", swapPort);
		exit(1);

	} else {
		log_info(logg,
				"A la escucha de algún proceso Administrador de Memoria el puerto: %d.", swapPort);
	}
}

//FUNCIONES DE COMUNICACIÓN

void mem_sw_startProcess(socket_connection *conn, char **args){

		int pid = atoi(args[0]);
		int pq  = atoi(args[1]);

		startProcess(pid,pq);
}

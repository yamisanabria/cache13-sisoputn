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
#include <errno.h>
#include <socket.h>
#include <utils.h>

#include "shared.h"

/** Variables generales que usaremos **/
t_dictionary * callableRemoteFunctions;	/* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/
int		socket_swap = -1;
int		socket_cpu 	= -1;

/** Datos de configuración **/
int		memoryPort;
int 	swapPort;
char* 	swapIP;

void initConnections(t_config* memoryConfig)
{
	memoryPort 		= config_get_int_value(memoryConfig, "PUERTO_ESCUCHA");
	swapPort 		= config_get_int_value(memoryConfig, "PUERTO_SWAP");
	swapIP 			= config_get_string_value(memoryConfig, "IP_SWAP");

	/* Agregamos las funciones que podrán ser llamadas por mis conexiones */
	callableRemoteFunctions = dictionary_create();
}

void cpuDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "CPU (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

void cpuNew(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Se ha conectado un nuevo CPU con ip %s en socket n°%d", socketInfo->ip, socketInfo->socket);
	log_info(logger, log_buffer);
}

void listenStart(){
	createListen(memoryPort, &cpuNew, callableRemoteFunctions, &cpuDisconnected, NULL);
	sprintf(log_buffer, "Nos ponemos en escucha de CPUs en puerto %d...", memoryPort);
	log_info(logger, log_buffer);
}

void swDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Swap (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

void connectSwap(){
	if(socket_swap != -1){
		return;
	} else {
		/* Me conecto al Swap; si hay error, informo y finalizo*/
		if((socket_swap = connectServer(swapIP, swapPort, callableRemoteFunctions, &swDisconnected, NULL)) == -1)
		{
			log_error(logger, "Error al intentar conectar con el Swap");

		} else {
			sprintf(log_buffer, "Se ha conectado exitosamente al Swap en IP %s en puerto %d por socket n° %d", swapIP, swapPort, socket_swap);
			log_info(logger, log_buffer);
		}
	}
}

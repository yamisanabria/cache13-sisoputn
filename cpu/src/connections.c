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
int		socket_scheduler= -1;
int		socket_memory 	= -1;

/** Datos de configuración **/
int 	schedulerPort;
char* 	schedulerIP;
int 	memoryPort;
char* 	memoryIP;

void setConnectionsParameters(t_config* martaConfig)
{
	schedulerPort 	= config_get_int_value(martaConfig, "PUERTO_PLANIFICADOR");
	schedulerIP 	= config_get_string_value(martaConfig, "IP_PLANIFICADOR");
	memoryPort 		= config_get_int_value(martaConfig, "PUERTO_MEMORIA");
	memoryIP 		= config_get_string_value(martaConfig, "IP_MEMORIA");
}

void scDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Scheduler (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

void memDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Memory (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

void connectScheduler(){
	if(socket_scheduler != -1){
		return;
	} else {
		/* Me conecto al FileSystem; si hay error, informo y finalizo*/
		if((socket_scheduler = connectServer(schedulerIP, schedulerPort, callableRemoteFunctions, &scDisconnected, NULL)) == -1)
		{
			log_error(logger, "Error al intentar conectar con el scheduler");
		} else {
			sprintf(log_buffer, "Se ha conectado exitosamente al Scheduler en IP %s en puerto %d por socket n° %d", schedulerIP, schedulerPort, socket_scheduler);
			log_info(logger, log_buffer);
		}
	}
}

void connectMemory(){
	if(socket_memory != -1){
		return;
	} else {
		/* Me conecto al FileSystem; si hay error, informo y finalizo*/
		if((socket_memory = connectServer(memoryIP, memoryPort, callableRemoteFunctions, &memDisconnected, NULL)) == -1)
		{
			log_error(logger, "Error al intentar conectar con el memory");

		} else {
			sprintf(log_buffer, "Se ha conectado exitosamente al Memory en IP %s en puerto %d por socket n° %d", memoryIP, memoryPort, socket_memory);
			log_info(logger, log_buffer);
		}
	}
}

void initializeRemoteFunctions()
{
	/* Agregamos las funciones que podrán ser llamadas por mis conexiones */
	callableRemoteFunctions = dictionary_create();

}

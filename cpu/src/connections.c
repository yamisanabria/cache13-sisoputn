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
#include "conncommands.h"

/** Variables generales que usaremos **/
t_dictionary * callableSchedulerRemoteFunctions;	/* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/
t_dictionary * callableMemoryRemoteFunctions;	/* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/

/** Datos de configuración **/
int 	schedulerPort;
char* 	schedulerIP;
int 	memoryPort;
char* 	memoryIP;

void setConnectionsParameters(t_config* config)
{
	schedulerPort 	= config_get_int_value(config, "PUERTO_PLANIFICADOR");
	schedulerIP 	= config_get_string_value(config, "IP_PLANIFICADOR");
	memoryPort 		= config_get_int_value(config, "PUERTO_MEMORIA");
	memoryIP 		= config_get_string_value(config, "IP_MEMORIA");
}

void scDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Scheduler (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
	exit(1);
}

void memDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Memory (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
	exit(1);
}

int connectScheduler(){
	/* Me conecto al FileSystem; si hay error, informo y finalizo*/
	int socket_scheduler = connectServer(schedulerIP, schedulerPort, callableSchedulerRemoteFunctions, &scDisconnected, NULL);
	if(socket_scheduler == -1) {
		sprintf(log_buffer, "Error al intentar conectar con el scheduler en IP %s en puerto %d", schedulerIP, schedulerPort);
		log_error(logger, log_buffer);
		exit(1);
	} else {
		sprintf(log_buffer, "Se ha conectado exitosamente al Scheduler en IP %s en puerto %d por socket n° %d", schedulerIP, schedulerPort, socket_scheduler);
		log_info(logger, log_buffer);
	}
	return socket_scheduler;

}

int connectMemory(){
	/* Me conecto al FileSystem; si hay error, informo y finalizo*/
	int socket_memory = connectServer(memoryIP, memoryPort, callableMemoryRemoteFunctions, &memDisconnected, NULL);
	if (socket_memory == -1) {
		sprintf(log_buffer, "Error al intentar conectar con el Memory en IP %s en puerto %d", memoryIP, memoryPort);
		log_error(logger, log_buffer);
		exit(1);
	} else {
		sprintf(log_buffer, "Se ha conectado exitosamente al Memory en IP %s en puerto %d por socket n° %d", memoryIP, memoryPort, socket_memory);
		log_info(logger, log_buffer);
	}
	return socket_memory;
}

void initializeRemoteFunctions()
{
	/* Agregamos las funciones que podrán ser llamadas por mis conexiones */
	callableSchedulerRemoteFunctions = dictionary_create();
	callableMemoryRemoteFunctions = dictionary_create();


	// llamadas desde el scheduler
	dictionary_put(callableSchedulerRemoteFunctions, "sc_cpu_startProcess", &schedulerStartProcess);
	dictionary_put(callableSchedulerRemoteFunctions, "sc_cpu_getStats", &schedulerGetStats);

	// desde el scheduler recibimos el mensaje sc_cpu_startProcess que inicia la ejecucion de un proceso
	// luego le avisamos al administrador de memoria cpu_mem_startProcess(pid, pages)
	// y luego de recibir del administrado de memoria uno de los mensajes;
	// (mem_cpu_startProcessOk, mem_cpu_noFrames o memoryNoSpace)
	// si es el primero paso a ejucutar el .cod
	// si es cualquiera de los otros tengo que informarle la falla al scheduler


	// llamadas desde el adm de memoria
	dictionary_put(callableMemoryRemoteFunctions, "mem_cpu_startProcessOk", &memoryStartProcessOk);
	dictionary_put(callableMemoryRemoteFunctions, "mem_cpu_noFrames", &memoryNoFrames);
	dictionary_put(callableMemoryRemoteFunctions, "mem_cpu_noSpace", &memoryNoSpace);
	dictionary_put(callableMemoryRemoteFunctions, "mem_cpu_frameData", &memoryFrameData);
	dictionary_put(callableMemoryRemoteFunctions, "mem_cpu_writeOk", &memoryWriteOk);

}

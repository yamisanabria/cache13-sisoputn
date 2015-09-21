#include "shared.h"

/** Variables generales que usaremos **/
t_dictionary * callableRemoteFunctions;	/* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/

/** Datos de configuración **/
int 	schedulerPort;

void setConnectionsParameters(t_config* schedulerConfig)
{
	schedulerPort 		= config_get_int_value(schedulerConfig, "PUERTO_ESCUCHA");
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

void listenStart()
{
	createListen(schedulerPort, &cpuNew, callableRemoteFunctions, &cpuDisconnected, NULL);
	sprintf(log_buffer, "Nos ponemos en escucha de CPUs en puerto %d...", schedulerPort);
	log_info(logger, log_buffer);
}

#include "shared.h"
#include "cpu.h"

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

	//todo, matamos?
	sprintf(log_buffer, "Cierro programa.\n");
	log_error(logger, log_buffer);

	exit(1);
}

void cpuNew(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Se ha conectado un nuevo CPU con ip %s en socket n°%d", socketInfo->ip, socketInfo->socket);
	log_info(logger, log_buffer);

	CPU* cpu = malloc(sizeof(CPU));
	cpu->socket			= socketInfo;
	cpu->status			= CPU_AVAILABLE;

	addNewCPU(cpu);
}

void cpuRunProcess(CPU* cpu){
	sprintf(log_buffer, "Llamando a startProcess en CPU %d con (%d, %s, %d, %d) en socket n°%d", cpu->id, cpu->process->pid, cpu->process->path, cpu->process->counter, P_QUANTUM, cpu->socket->socket);
	log_info(logger, log_buffer);
	runFunction(cpu->socket->socket, "sc_cpu_startProcess", 4, cpu->process->path, cpu->process->pid, string_itoa(cpu->process->counter), string_itoa(P_QUANTUM));
}

void listenStart()
{
	createListen(schedulerPort, &cpuNew, callableRemoteFunctions, &cpuDisconnected, NULL);
	sprintf(log_buffer, "Nos ponemos en escucha de CPUs en puerto %d...", schedulerPort);
	log_info(logger, log_buffer);
}

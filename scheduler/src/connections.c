#include "shared.h"
#include "cpu.h"
#include "pcb.h"

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

/* ################### SERVIDOR ################### */
/**
 *
 */
void cpuProcessIsBack(socket_connection * connection, char ** args)
{
	int pid 		= atoi(args[0]);
	int status 		= atoi(args[1]);
	char* messages 	= string_duplicate(args[2]);
	char* data 		= args[3];

	CPU* cpu = findCPUBySocketConnection(connection);
	PCBItem* process = pcbGetByPID(pid);

	sprintf(log_buffer, "El proceso %d ha vuelto del CPU %d", pid, cpu->id);
	log_info(logger, log_buffer);

	cpuPrintMessages(cpu, process, messages);

	switch(status){
		case 1: //Ráfaga ok
			processHasFinishedBurst(process);
			break;
		case 2: //Falló
			processHasFailed(process);
			break;
		case 3: //Terminó
			processHasFinished(process);
			break;
		case 4: //Bloqueado
			int sleep_time = atoi(data);
			processHasBeenBlocked(process, sleep_time);
			break;
		default:
			sprintf(log_buffer, "El estado %d es inválido", status);
			log_error(logger, log_buffer);
			exit(1);
	}

	markCPUAsAvailable(cpu);

	checkReadyProcesses();
}

/* ################### CLIENTE ################### */

/**
 * Le informamos a un CPU que tiene que ejecutar un proceso
 * Si el quantum es 0 es que tiene que ejecutar hasta que corte (FIFO)
 */
void cpuRunProcess(CPU* cpu){
	sprintf(log_buffer, "Llamando a startProcess en CPU %d con (%d, %s, %d, %d) en socket n°%d", cpu->id, cpu->process->PID, cpu->process->path, cpu->process->counter, P_QUANTUM, cpu->socket->socket);
	log_info(logger, log_buffer);
	runFunction(cpu->socket->socket, "sc_cpu_startProcess", 4, cpu->process->path, cpu->process->PID, string_itoa(cpu->process->counter), string_itoa(P_QUANTUM));
}

void listenStart()
{
	callableRemoteFunctions = dictionary_create();
	dictionary_put(callableRemoteFunctions, "cpu_sc_process_back", &cpuProcessIsBack);

	createListen(schedulerPort, &cpuNew, callableRemoteFunctions, &cpuDisconnected, NULL);
	sprintf(log_buffer, "Nos ponemos en escucha de CPUs en puerto %d...", schedulerPort);
	log_info(logger, log_buffer);
}

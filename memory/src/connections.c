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
#include "memory.h"

/** Variables generales que usaremos **/
t_dictionary * callableRemoteFunctions;	/* Diccionario de funciones que pueden ser llamadas por mis conexiones (FUNCIONES SERVIDOR)*/
int		socket_swap = -1;


/** Datos de configuración **/
int		memoryPort;
int 	swapPort;
char* 	swapIP;


//-###############################################################################################-//
//-###-[GENERAL]-#################################################################################-//
//-###############################################################################################-//

// CPU se desconecta
void cpuDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "CPU (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

// CPU se conecta
void cpuNew(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Se ha conectado un nuevo CPU con ip %s en socket n°%d", socketInfo->ip, socketInfo->socket);
	log_info(logger, log_buffer);
}

// Comeinza escucha de CPUs
void listenStart(){
	createListen(memoryPort, &cpuNew, callableRemoteFunctions, &cpuDisconnected, NULL);
	sprintf(log_buffer, "Nos ponemos en escucha de CPUs en puerto %d...", memoryPort);
	log_info(logger, log_buffer);
}

// Swap se desconecta
void swDisconnected(socket_connection* socketInfo)
{
	sprintf(log_buffer, "Swap (socket n°%d) se ha desconectado.\n", socketInfo->socket);
	log_info(logger, log_buffer);
}

// Se conecta al swap
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


//-###############################################################################################-//
//-###-[OUTPUT]-##################################################################################-//
//-###############################################################################################-//

//-###########################################-//
//-###-[CPU]-#################################-//
//-###########################################-//

// Indicamos que se comenzo correctamente el proceso
void cpu_startProcessOk(int socket, int pid)
{
	char * pid_str = string_itoa(pid);
	runFunction(socket, "mem_cpu_startProcessOk", 1, pid_str);
	free(pid_str);
}

// Indicamos que no hay frames suficientes para comenzar un proceso
void cpu_noFrames(int socket, int pid)
{
	char * pid_str = string_itoa(pid);
	runFunction(socket, "mem_cpu_noFrames", 1, pid_str);
	free(pid_str);
}

// Indicamos que no hay suficiente espacio para comenzar un proceso
void cpu_noSpace(int socket, int pid)
{
	char * pid_str = string_itoa(pid);
	runFunction(socket, "mem_cpu_noSpace", 1, pid_str);
	free(pid_str);
}

// Enviamos contenido de un frame
void cpu_frameData(int socket, int frame, char * data)
{
	log_info(logger, "%d", frame);
	log_info(logger, data);
	char * frame_str = string_itoa(frame);
	runFunction(socket, "mem_cpu_frameData", 2, frame_str, data);
	free(frame_str);
}

// Indicamos que se realizo la escritura
void cpu_writeOk(int socket)
{
	runFunction(socket, "mem_cpu_writeOk", 0);
}


//-###########################################-//
//-###-[SWAP]-################################-//
//-###########################################-//

//Le indicamos que comienza un proceso con la necesidad de páginas
void sw_startProcess(int pid, int pages)
{
	char * pid_str = string_itoa(pid);
	char * pages_str = string_itoa(pages);
	runFunction(socket_swap, "mem_sw_startProcess", 2, pid_str, pages_str);
	free(pid_str);
	free(pages_str);
}

//Solicitamos una pagina
void sw_getPage(int pid, int page)
{
	char * pid_str = string_itoa(pid);
	char * page_str = string_itoa(page);
	runFunction(socket_swap, "mem_sw_getPage", 2, pid_str, page_str);
	free(pid_str);
	free(page_str);
}

//Escribimos una pagina
void sw_setPage(int pid, int page, char *data)
{
	char * pid_str = string_itoa(pid);
	char * page_str = string_itoa(page);
	runFunction(socket_swap, "mem_sw_setPage", 3, pid_str, page_str, data);
	free(pid_str);
	free(page_str);
}

//Hacemos intercambio de paginas
void sw_swapping(int pid, int setPage, char * data, int getPageNum)
{
	char * pid_str = string_itoa(pid);
	char * setPage_str = string_itoa(setPage);
	char * getPage_str = string_itoa(getPageNum);
	runFunction(socket_swap, "mem_sw_swapping", 4, pid_str, setPage_str, data, getPage_str);
	free(pid_str);
	free(setPage_str);
	free(getPage_str);
}

//Indicamos la finalizacion de un proceso
void sw_endProcess(int pid)
{
	char * pid_str = string_itoa(pid);
	runFunction(socket_swap, "mem_sw_endProcess", 1, pid_str);
	free(pid_str);
}

//-###############################################################################################-//
//-###-[INPUT]-###################################################################################-//
//-###############################################################################################-//

//-###########################################-//
//-###-[CPU]-#################################-//
//-###########################################-//

// Nos indica que comienza un proceso
void cpu_startProcess(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso
	int pages = atoi(args[1]);		// cantidad de páginas necesarios para el proceso

	write_start();

	log_info(logger, "CPU solicita comenzar proceso PIDº%d (%d páginas)", pid, pages);

	addProcess(pid, pages, connection);
	sw_startProcess(pid, pages);

	write_end();
}

// Nos solicita la lectura de un frame
void cpu_read(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso
	int page = atoi(args[1]);		// número de página a leer

	write_start();

	log_info(logger, "CPU solicita lectura de página Nº%d (PIDº%d)",page, pid);

	addReadPetition(pid, page, connection);

	write_end();
}

// Nos indica escribir un frame
void cpu_write(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);					// pid del proceso
	int page = atoi(args[1]);					// número de página a escribir
	char * data = string_duplicate(args[2]);	// página a escribir en el frame

	write_start();

	log_info(logger, "CPU solicita escribir en página Nº%d (PIDº%d)", page, pid);

	addWritePetition(pid, page, data, connection);

	write_end();
}

// Nos indica que finalizo un proceso
void cpu_endProcess(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso

	write_start();

	log_info(logger, "CPU solicita finalizar proceso PID Nº%d", pid);

	sw_endProcess(pid);
	deleteProcess(pid);

	write_end();
}

//-###########################################-//
//-###-[SWAP]-################################-//
//-###########################################-//

// Nos indica que comenzo correctamente el proceso
void sw_startProcessOk(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso

	read_start();

	log_info(logger, "SWAP nos indica que comenzo correctamente el proceso PIDº%d", pid);

	t_process * process = getProcess(pid);
	cpu_startProcessOk(process->connection->socket, pid);

	read_end();
}

// Nos indica que no tiene suficiente espacio para el proceso a comenzar
void sw_noSpace(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso

	write_start();

	log_info(logger, "SWAP nos indica que no tiene suficiente espacio para comenzar el proceso PIDº%d", pid);

	t_process * process = getProcess(pid);
	deleteProcess(pid);
	cpu_noSpace(process->connection->socket, pid);

	write_end();
}

// Nos devuelve el conenido de una pagina
void sw_page(socket_connection * connection, char ** args)
{
	int pid = atoi(args[0]);		// pid del proceso
	int page_num = atoi(args[1]);	// número de página a escribir
	char * data = args[2];			// datos de la pagina

	write_start();

	log_info(logger, "SWAP nos devuelve el conenido de una pagina Nº%d (PIDº%d)", page_num, pid);

	t_page * page = getPage(pid, page_num);
	setMemoryData(page->frame, data, false);
	page->present = true;
	runPetitions();

	write_end();
}



void initConnections(t_config* memoryConfig)
{
	memoryPort 		= config_get_int_value(memoryConfig, "PUERTO_ESCUCHA");
	swapPort 		= config_get_int_value(memoryConfig, "PUERTO_SWAP");
	swapIP 			= config_get_string_value(memoryConfig, "IP_SWAP");

	/* Agregamos las funciones que podrán ser llamadas por mis conexiones */
	callableRemoteFunctions = dictionary_create();
	//cpu
	dictionary_put(callableRemoteFunctions, "cpu_mem_startProcess", &cpu_startProcess);
	dictionary_put(callableRemoteFunctions, "cpu_mem_read", &cpu_read);
	dictionary_put(callableRemoteFunctions, "cpu_mem_write", &cpu_write);
	dictionary_put(callableRemoteFunctions, "cpu_mem_endProcess", &cpu_endProcess);
	//swap
	dictionary_put(callableRemoteFunctions, "sw_mem_startProcessOk", &sw_startProcessOk);
	dictionary_put(callableRemoteFunctions, "sw_mem_noSpace", &sw_noSpace);
	dictionary_put(callableRemoteFunctions, "sw_mem_page", &sw_page);
}

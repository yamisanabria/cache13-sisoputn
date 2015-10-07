	// desde el scheduler recibimos el mensaje sc_cpu_startProcess que inicia la ejecucion de un proceso
	// luego le avisamos al administrador de memoria cpu_mem_startProcess(pid, pages) 
	// y luego de recibir del administrado de memoria uno de los mensajes;
	// (mem_cpu_startProcessOk, mem_cpu_noFrames o memoryNoSpace)
	// si es el primero paso a ejucutar el .cod
	// si es cualquiera de los otros tengo que informarle la falla al scheduler

#include <stdlib.h>
#include <socket.h>
#include <commons/string.h>

#include "shared.h"
#include "cpumanager.h"
#include "config.h"


void schedulerStartProcess(socket_connection* connection, char ** args){
	char* path 			= args[0];		// path del codigo
	int pid = atoi(args[1]);// linea a ejecutar
	int process_counter = atoi(args[1]);// linea a ejecutar
	int quantum			= atoi(args[2]);// cantidad de lineas a ejectuar , si es 0 ejectua todo hasta cortar


	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	free(cpu->codfile);
	free(cpu->execResponseBuffer);
	
	// setear datos en cpu para despues correr
	cpu->execResponseBuffer = string_new();
	cpu->codfile = string_duplicate(path);
	cpu->execPid = pid;
	cpu->process_counter = process_counter;
	cpu->quantum = quantum;

	// pedimos la memoria a cache13 para correr el .cod
	runFunction(cpu->socketIdMemory, "cpu_mem_startProcess", 2, pid, /* De donde caxo saco las paginas?? */ 3);
}

void memoryStartProcessOk(socket_connection* connection, char ** args) {

}

void memoryNoFrames(socket_connection* connection, char ** args) {

}

void memoryNoSpace(socket_connection* connection, char ** args) {

}

void memoryFrameData(socket_connection* connection, char ** args) {

}

void memoryWriteOk(socket_connection* connection, char ** args) {

}
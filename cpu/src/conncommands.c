#include <stdlib.h>
#include <socket.h>
#include <commons/string.h>
#include <commons/config.h>

#include "shared.h"
#include "cpumanager.h"
#include "codparser.h"

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
	
	// si el quantum es 0 significa que es apropiativa
	// o sea corre hasta fin o entrada salida, a fines
	// practiocos voy a usar -1 si quantum es 0
	if (quantum==0) {
		cpu->quantum = -1;
	} else {
		cpu->quantum = quantum;
	}

	consumeQuantum(cpu);
	
}

void memoryStartProcessOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	// TODO
	// hacer lo que haya que hacer y seguir consumiendo quantums;
	//consumeQuantums(cpu);
}

void memoryNoFrames(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	// TODO
	// hacer lo que haya que hacer y seguir consumiendo quantums;

}

void memoryNoSpace(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	// TODO
	// hacer lo que haya que hacer y seguir consumiendo quantums;

}

void memoryFrameData(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);
	// TODO
	// hacer lo que haya que hacer y seguir consumiendo quantums;
	//consumeQuantums(cpu);
}

void memoryWriteOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);
	// TODO
	// hacer lo que haya que hacer y seguir consumiendo quantums;
	//consumeQuantums(cpu);
}
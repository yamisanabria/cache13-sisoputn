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
	int process_counter = atoi(args[2]);// linea a ejecutar
	int quantum			= atoi(args[3]);// cantidad de lineas a ejectuar , si es 0 ejectua todo hasta cortar


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

void schedulerGetStats(socket_connection* connection, char ** args){
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);
	// TODO por ahora devuelve siempre 1, hay que acordar que debe hacer
	runFunction(cpu->socketIdScheduler, "cpu_sc_stats", 1, string_itoa(1));
}


void memoryStartProcessOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _buffer = string_from_format("mProc %s - Iniciado\n", string_itoa(cpu->execPid));
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	cpu->process_counter = cpu->process_counter + 1;
	// hacer lo que haya que hacer y seguir consumiendo quantums;
	consumeQuantum(cpu);
}

void memoryNoFrames(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _buffer = string_from_format("mProc %s - Falló: Espacion en swap insuficiente\n", string_itoa(cpu->execPid));
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, string_itoa(cpu->execPid), "2", string_itoa(cpu->process_counter + 1), string_duplicate(cpu->execResponseBuffer), "no_frames");

}

void memoryNoSpace(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _buffer = string_from_format("mProc %s - Falló: Espacio en memoria insuficiente\n", string_itoa(cpu->execPid));
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, string_itoa(cpu->execPid), "2", string_itoa(cpu->process_counter + 1), string_duplicate(cpu->execResponseBuffer), "no_space");

}

void memoryFrameData(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _frame = string_duplicate(args[0]);
	char* _data = string_duplicate(args[1]);

	char* _buffer = string_from_format("mProc %s - Página %s leida: %s\n", string_itoa(cpu->execPid), _frame, _data);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	free(_data);
	free(_frame);

	cpu->process_counter = cpu->process_counter + 1;
	consumeQuantum(cpu);
}

void memoryWriteOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	cpu->process_counter = cpu->process_counter + 1;
	consumeQuantum(cpu);
}

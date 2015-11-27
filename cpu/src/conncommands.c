#include <stdlib.h>
#include <socket.h>
#include <commons/string.h>
#include <commons/config.h>

#include "shared.h"
#include "cpumanager.h"
#include "codparser.h"
#include "utils.h"

void schedulerStartProcess(socket_connection* connection, char ** args){
	char* path 			= args[0];		// path del codigo
	int pid = atoi(args[1]);// linea a ejecutar
	int process_counter = atoi(args[2]);// linea a ejecutar
	int quantum			= atoi(args[3]);// cantidad de lineas a ejectuar , si es 0 ejectua todo hasta cortar


	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	// setear datos en cpu para despues correr
	free(cpu->execResponseBuffer);
	cpu->execResponseBuffer = string_new();
	free(cpu->codfile);
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

	updateCpuTimer(cpu);
	consumeQuantum(cpu);

}

void schedulerGetStats(socket_connection* connection, char ** args){
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuBySchedulerSocket(connection->socket);

	float percentage = 0;
	unsigned long long temp = 0;
	unsigned long long now = getTimeNow();

	bool _hacemasDeUnMinuto(QUANTUMstat *s) {
		return s->end && s->end < (now - (unsigned long long)(60*1000));
	}
	list_remove_and_destroy_by_condition(cpu->rawstats, (void*) _hacemasDeUnMinuto, free);

	void _calculateLastMinutePercentage(QUANTUMstat* st){
		unsigned long long t1 = st->end;
		unsigned long long t2 = st->init;
		if(!t1)
			t1 = now;
		if(st->init < now - (unsigned long long)(60*1000))
			t2 = now - (unsigned long long)(60*1000);
		temp += t1 - t2;
	}

	list_iterate(cpu->rawstats, (void*)_calculateLastMinutePercentage);

	percentage = ((float)temp / (float)(60*1000)) * 100;

	if(percentage >= 100)
		percentage = 100;

	char* p = string_itoa(percentage);
	runFunction(cpu->socketIdScheduler, "cpu_sc_stats", 1, p);
	free(p);
}


void memoryStartProcessOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* pid = string_itoa(cpu->execPid);
	char* _buffer = string_from_format("mProc %s - Iniciado\n", pid);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);
	free(pid);

	cpu->process_counter = cpu->process_counter + 1;
	// hacer lo que haya que hacer y seguir consumiendo quantums;
	updateCpuTimer(cpu);
	consumeQuantum(cpu);
}

void memoryNoFrames(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _buffer = string_from_format("mProc %s - Falló: Espacion en swap insuficiente\n", string_itoa(cpu->execPid));
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	updateCpuTimer(cpu);
	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, string_itoa(cpu->execPid), "2", string_itoa(cpu->process_counter + 1), string_duplicate(cpu->execResponseBuffer), "no_frames");
}

void memoryNoSpace(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);
	char* pid = string_itoa(cpu->execPid);
	char* _buffer = string_from_format("mProc %s - Falló: Espacio en memoria insuficiente\n", pid);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);
	free(pid);

	updateCpuTimer(cpu);
	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, pid, "2", string_itoa(cpu->process_counter + 1), string_duplicate(cpu->execResponseBuffer), "no_space");
}

void memoryFrameData(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets
	CPU* cpu = findCpuByMemorySocket(connection->socket);

	char* _frame = string_duplicate(args[0]);
	char* _data = string_duplicate(args[1]);
	char* pid = string_itoa(cpu->execPid);
	char* _buffer = string_from_format("mProc %s - Página %s leida: %s\n", pid, _frame, _data);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);
	free(pid);
	free(_data);
	free(_frame);

	cpu->process_counter = cpu->process_counter + 1;
	updateCpuTimer(cpu);
	consumeQuantum(cpu);
}

void memoryWriteOk(socket_connection* connection, char ** args) {
	//Identificar CPU comparando sockets

	CPU* cpu = findCpuByMemorySocket(connection->socket);

	cpu->process_counter = cpu->process_counter + 1;
	updateCpuTimer(cpu);
	consumeQuantum(cpu);
}

#include "shared.h"

t_list* cpus;
int _cpuindex = 0;

void createCPUsList(){
	cpus = list_create();
}

t_list* getCPUsList(){
	return cpus;
}

void addNewCPU(CPU* cpu)
{
	_cpuindex ++;
	cpu->id = _cpuindex;
	list_add(cpus, cpu);
}

CPU* getCPUByID(int id)
{
	return list_get(cpus, id);
}

void markCPUAsAvailable(CPU* cpu)
{
	cpu->status 	= CPU_AVAILABLE;
	cpu->process	= NULL;
}

bool _cpuIsAvailable(void *cpu) {
    return ((CPU *)cpu)->status == CPU_AVAILABLE;
}

bool isAnyCPUAvailable() {
	return list_any_satisfy(cpus, (void*)_cpuIsAvailable);
}

CPU* findCPUBySocketConnection(socket_connection* socket){
	bool _find(CPU* cpu) { return cpu->socket->socket == socket->socket; }
	return list_find(cpus, (void*) _find);
}

CPU* findCPUAvailable(){
	return list_find(cpus, (void*)_cpuIsAvailable);
}

void cpuPrintMessages(CPU* cpu, PCBItem* process, char* messages)
{
	sprintf(log_buffer, "Imprimimos mensajes del CPU %d, PID %d", cpu->id, process->PID);
	log_info(logger, log_buffer);

	printf("%s", messages);
}

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>

#include "shared.h"
#include "connections.h"
#include "cpumanager.h" // ?? creo que no es necesario

t_list* cpuList;

void initCpuManager(){
	cpuList = list_create();
}

void createCpu(){
	CPU* _cpu = malloc(sizeof(CPU));

	_cpu->status = CPU_AVAILABLE;
	_cpu->socketIdMemory = connectMemory();
	_cpu->socketIdScheduler = connectScheduler();

	_cpu->rawstats = list_create();

	list_add(cpuList, _cpu);
}

CPU* findCpuBySchedulerSocket(int socket_id){
	bool _haveSchedulerSocket(CPU* _cpu) {
		return _cpu->socketIdScheduler == socket_id;
	}

	return (CPU*) list_find(cpuList, (void*) _haveSchedulerSocket);
}

CPU* findCpuByMemorySocket(int socket_id){
	bool _haveMemorySocket(CPU* _cpu) {
		return _cpu->socketIdMemory == socket_id;
	}

	return (CPU*) list_find(cpuList, (void*) _haveMemorySocket);
}

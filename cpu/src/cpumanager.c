#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>

#include "shared.h"
#include "connections.h"
#include "cpumanager.h"

t_list* cpuList;

void initCpuManager(){
	cpuList = list_create();	
}

void createCpu(){
	CPU* _cpu = malloc(sizeof(CPU));

	_cpu->socketIdScheduler = connectScheduler();
	_cpu->socketIdMemory = connectMemory();
	_cpu->status = CPU_AVAILABLE;
	
	list_add(cpuList, _cpu);
}

CPU* findCpuBySchedulerSocket(int socket_id){
	int _haveSchedulerSocket(CPU* _cpu) {
		return _cpu->socketIdScheduler == socket_id;
	}

	return (CPU*) list_find(cpuList, (void*) _haveSchedulerSocket);
}

CPU* findCpuByMemorySocket(int socket_id){
	int _haveMemorySocket(CPU* _cpu) {
		return _cpu->socketIdMemory == socket_id;
	}

	return (CPU*) list_find(cpuList, (void*) _haveMemorySocket);
}
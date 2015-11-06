#include "shared.h"
#include "cpu.h"
#include "connections.h"

t_queue* pQueue;
pthread_mutex_t mutex_checkReadyProcesses;

void createProcessQueue(){
	pQueue = queue_create();
}

void pQueueAddProcess(PCBItem* process){

	queue_push(pQueue, process);
}

void pQueueAddProcessFirst(PCBItem* process){
	t_queue* tmp = queue_create();
	while(!queue_is_empty(pQueue)){
		queue_push(tmp, queue_pop(pQueue));
	}
	queue_push(pQueue, process);
	while(!queue_is_empty(tmp)){
		queue_push(pQueue, queue_pop(tmp));
	}
	free(tmp);
}


void assignProcessToCPU(CPU* cpu, PCBItem* item) {
	item->status = P_RUNNING;
	cpu->status = CPU_BUSY;
	cpu->process = item;

	cpuRunProcess(cpu);
}

void checkReadyProcesses(){

	pthread_mutex_lock(&mutex_checkReadyProcesses);

	if(!queue_is_empty(pQueue) && isAnyCPUAvailable()){

		CPU* cpu = findCPUAvailable();
		PCBItem* item = queue_pop(pQueue);

		sprintf(log_buffer, "PROCESO PID-%d RUNNING.\n", item->PID);
		log_info(logger, log_buffer);

		assignProcessToCPU(cpu, item);
	}

	pthread_mutex_unlock(&mutex_checkReadyProcesses);
}

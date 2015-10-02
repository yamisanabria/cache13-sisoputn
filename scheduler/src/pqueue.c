#include "shared.h"
#include "cpu.h"

//Asumimos FIFO

t_queue* pQueue;
pthread_mutex_t mutex_checkReadyProcesses;

void createProcessQueue(){
	pQueue = queue_create();
}

void pQueueAddProcess(PCBItem* process){

	process->status = P_READY;
	queue_push(pQueue, process);
}


void assignProcessToCPU(CPU* cpu, PCBItem* item) {
	// TODO
}

void checkReadyProcesses(){

	pthread_mutex_lock(&mutex_checkReadyProcesses);

	if(!queue_is_empty(pQueue) && isAnyCPUAvailable()){

		CPU* cpu = findCPUAvailable();
		PCBItem* item = queue_pop(pQueue);

		assignProcessToCPU(cpu, item);
	}

	pthread_mutex_unlock(&mutex_checkReadyProcesses);
}

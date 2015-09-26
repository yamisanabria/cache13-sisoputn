#include "shared.h"
#include "unistd.h"

t_list* pcb;

void createPCB(){
	pcb = list_create();
}

int validateCodFile(char* path){
	if(access(path, F_OK) == -1){
		sprintf(log_buffer, "El archivo (%s) no existe.\n", path);
		log_info(logger, log_buffer);
		return 0;
	} else {
		return 1;
	}
}

int pcbAddNewProcess(PCBItem* item)
{
	int pid 		= list_size(pcb) + 1;
	item->PID 		= pid;
	item->status	= P_READY;
	list_add(pcb, item);
	return pid;
}

void checkReadyProcesses(){

}

int runNewProcess(char* path){

	if(validateCodFile(path)){
		PCBItem* item 	= malloc(sizeof(PCBItem));
		item->counter 	= 1;
		item->path 		= string_duplicate(path);
		item->status 	= P_NEW;
		item->start 	= clock();

		int pid = pcbAddNewProcess(item);
		pQueueAddProcess(PCBItem);
		sprintf(log_buffer, "PROCESO PID-%d READY.\n", pid);
		log_info(logger, log_buffer);

		checkReadyProcesses();

		return 1;
	} else {
		return 0;
	}
}

PCBItem* pcbGetByPID(int pid)
{
	return list_get(pcb, pid);
}

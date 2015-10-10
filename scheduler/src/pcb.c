#include "shared.h"
#include "unistd.h"
#include "pqueue.h"
#include "ioqueue.h"

t_list* pcb;

void createPCB()
{
	pcb = list_create();
}

t_list* getPCBlist()
{
	return pcb;
}

PCBItem* pcbGetByPID(int pid)
{
	return list_get(pcb, pid);
}

int validateCodFile(char* path)
{
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

int runNewProcess(char* path)
{
	if(validateCodFile(path)){
		PCBItem* item 	= malloc(sizeof(PCBItem));
		item->counter 	= 1;
		item->path 		= string_duplicate(path);
		item->status 	= P_NEW;
		item->start 	= clock();

		int pid = pcbAddNewProcess(item);
		item->status 	= P_READY;
		pQueueAddProcess(item);

		sprintf(log_buffer, "PROCESO PID-%d READY.\n", pid);
		log_info(logger, log_buffer);

		checkReadyProcesses();

		return 1;
	} else {
		return 0;
	}
}

void processHasFinished(PCBItem* item)
{
	item->status 	= P_FINISHED;
	item->end		= clock();
	sprintf(log_buffer, "PROCESO PID-%d FINISHED.\n", item->PID);
	log_info(logger, log_buffer);

	sprintf(log_buffer, "El proceso (PID:%d, %s) finalizó correctamente.\n", item->PID, item->path);
	log_info(logger, log_buffer);
}

void processHasFinishedBurst(PCBItem* item)
{
	item->status 	= P_READY;
	/**
	 * https://github.com/sisoputnfrba/foro/issues/83
	 * Esto es cualquiera...
	 */
	item->counter = item->counter + P_QUANTUM;

	sprintf(log_buffer, "PROCESO PID-%d READY.\n", item->PID);
	log_info(logger, log_buffer);

	pQueueAddProcess(item);
}

void processHasFailed(PCBItem* item)
{
	item->status 	= P_FAILED;
	item->end		= clock();
	sprintf(log_buffer, "PROCESO PID-%d FAILED.\n", item->PID);
	log_info(logger, log_buffer);

	sprintf(log_buffer, "El proceso (PID:%d, %s) terminó con errores.\n", item->PID, item->path);
	log_info(logger, log_buffer);
}

void processHasBeenBlocked(PCBItem* item, int sleep_time)
{
	item->status = P_BLOCKED;

	sprintf(log_buffer, "PROCESO PID-%d BLOCKED.\n", item->PID);
	log_info(logger, log_buffer);

	ioQueueAddProcess(item, sleep_time);
}

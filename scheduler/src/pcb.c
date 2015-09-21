#include "shared.h"

t_list* pcb;

void createPCB(){
	pcb = list_create();
}

void pcbAddNewProcess(PCBItem* item)
{
	int pid = list_size(pcb) + 1;
	item->PID = pid;
	list_add_in_index(pcb, item->PID, item);
}

PCBItem* pcbGetByPID(int pid)
{
	return list_get(pcb, pid);
}

#ifndef PCB_H_
#define PCB_H_

void createPCB();

void pcbAddNewProcess(PCBItem* item);

PCBItem* pcbGetByPID(int pid);

#endif

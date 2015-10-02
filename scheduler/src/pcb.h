#ifndef PCB_H_
#define PCB_H_

void createPCB();

t_list* getPCBlist();

int validateCodFile(char* path);

int pcbAddNewProcess(PCBItem* item);

int runNewProcess(char* path);

PCBItem* pcbGetByPID(int pid);

#endif

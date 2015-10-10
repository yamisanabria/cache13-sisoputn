#ifndef PCB_H_
#define PCB_H_

void createPCB();

t_list* getPCBlist();

int validateCodFile(char* path);

int pcbAddNewProcess(PCBItem* item);

int runNewProcess(char* path);

PCBItem* pcbGetByPID(int pid);

void processHasFinished(PCBItem* item);

void processHasFinishedBurst(PCBItem* item);

void processHasFailed(PCBItem* item);

void processHasFinished(PCBItem* item);

void processHasBeenBlocked(PCBItem* item, int sleep_time);

int finalizeProcess(char* pid);

int forceFinalize(PCBItem* item);

#endif

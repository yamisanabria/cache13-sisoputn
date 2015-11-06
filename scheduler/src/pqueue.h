#ifndef PQUEUE_H_
#define PQUEUE_H_

void createProcessQueue();

void pQueueAddProcess(PCBItem* process);

void pQueueAddProcessFirst(PCBItem* process);

void checkReadyProcesses();

#endif

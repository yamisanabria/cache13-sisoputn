#ifndef IOQUEUE_H_
#define IOQUEUE_H_

void createIOQueue();

void ioQueueAddProcess(PCBItem* process, int sleep_time);

void ioQueueRunner();

void ioQueueAddProcess(PCBItem* process, int sleep_time);

#endif

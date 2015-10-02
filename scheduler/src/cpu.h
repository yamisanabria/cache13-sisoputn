#ifndef CPU_H_
#define CPU_H_

void createCPUsList();

void addNewCPU(CPU* cpu);

CPU* getCPUByID(int id);

t_list* getCPUsList();

bool isAnyCPUAvailable();

CPU* findCPUAvailable();

#endif

#ifndef CPU_H_
#define CPU_H_

void addNewCPU(CPU* cpu);

CPU* getCPUByID(int id);

t_list* getCPUsList();

bool isAnyCPUAvailable();

CPU* findCPUAvailable();

#endif

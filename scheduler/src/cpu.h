#ifndef CPU_H_
#define CPU_H_

void createCPUsList();

void addNewCPU(CPU* cpu);

CPU* getCPUByID(int id);

t_list* getCPUsList();

bool isAnyCPUAvailable();

CPU* findCPUAvailable();

CPU* findCPUBySocketConnection(socket_connection* socket);

void markCPUAsAvailable(CPU* cpu);

void cpuPrintMessages(CPU* cpu, PCBItem* process, char* messages);

void cpuStatsAreHere(CPU* cpu, char* stats);

#endif

#include "shared.h"

t_list* cpus;

void createCPUsList(){
	cpus = list_create();
}

void addNewCPU(CPU* cpu)
{
	int id = list_size(cpus) + 1;
	cpu->id = id;
	list_add_in_index(cpus, cpu->id, cpu);
}

CPU* getCPUByID(int id)
{
	return list_get(cpus, id);
}

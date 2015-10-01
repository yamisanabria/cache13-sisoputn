#include "shared.h"

t_list* cpus;

void createCPUsList(){
	cpus = list_create();
}

t_list* getCPUsList(){
	return cpus;
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

bool _cpuIsAvailable(void *cpu) {
    return ((CPU *)cpu)->status == CPU_AVAILABLE;
}

bool isAnyCPUAvailable() {
	/*
	 Tomado de;
	 https://github.com/sisoputnfrba/so-commons-library/blob/5534cd0ff2d2b0ce28c22434e59c9bb0e37131a3/tests/unit-tests/test_list.c#L331
	*/
	return list_any_satisfy(cpus, (void*)_cpuIsAvailable);
}

CPU* findCPUAvailable(){
	CPU* _cpu;
	int i;

	for(i = 0; i < list_size(cpus); i++){
		_cpu = list_get(cpus, i);
		if (_cpuIsAvailable(_cpu)) {
			break;
		}
	}

	return _cpu;
}

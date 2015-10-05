#ifndef CPUMANAGER_H_
#define CPUMANAGER_H_

	void initCpuManager();

	void createCpu();

	CPU* findCpuBySchedulerSocket(int socket_id);

	CPU* findCpuByMemorySocket(int socket_id);
#endif
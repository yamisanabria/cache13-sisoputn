#ifndef SHARED_H_
#define SHARED_H_

	extern t_log* logger;
	extern char log_buffer[1024];

	typedef enum {CPU_AVAILABLE, CPU_BUSY} CPUStatus;

	typedef struct
	{
		int socketIdScheduler;
		int socketIdMemory;
		int execPid;
		char* execResponseBuffer;
		CPUStatus status;
	} CPU;

#endif

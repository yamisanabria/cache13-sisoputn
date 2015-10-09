#ifndef SHARED_H_
#define SHARED_H_


	#include <commons/collections/list.h>
	#include <commons/collections/dictionary.h>
	#include <commons/log.h>

	extern t_log* logger;
	extern char log_buffer[1024];

	typedef enum {CPU_AVAILABLE, CPU_BUSY} CPUStatus;

	typedef struct
	{
		int socketIdScheduler;
		int socketIdMemory;
		int execPid;
		int quantum;
		int process_counter;
		char* codfile;
		char* execResponseBuffer;
		CPUStatus status;
	} CPU;

#endif

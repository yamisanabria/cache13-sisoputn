#ifndef SHARED_H_
#define SHARED_H_

	extern t_log* logger;
	extern char log_buffer[1024];

	typedef struct
	{
		int socketIdScheduler;
		int socketIdMemory;
		char* execResponseBuffer;
	} CPU;

#endif

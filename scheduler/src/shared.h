#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <socket.h>
#include <utils.h>
#include <ctype.h>
#include <time.h>

#ifndef SHARED_H_
#define SHARED_H_

	extern t_log* logger;
	extern char log_buffer[1024];
	extern int P_QUANTUM;
	extern char* P_METHOD; //FIFO / RR

	typedef struct
	{
		char * name;
		void(*fn)();
		int args;
		char * description;
	} Command;

	typedef enum {P_NEW, P_READY, P_RUNNING, P_BLOCKED, P_FAILED, P_FINISHED} PCBStatus;

	typedef struct
	{
		int PID;
		char* path;
		PCBStatus status;
		int counter;		//Número de próxima línea a ejecutar
		clock_t start;
		clock_t end;
	} PCBItem;

	typedef enum {CPU_AVAILABLE, CPU_BUSY} CPUStatus;

	typedef struct
	{
		int id;
		CPUStatus status;
		socket_connection* socket;
		clock_t last_start;
		clock_t last_end;
		PCBItem* process; //Proceso que está corriendo
	} CPU;
#endif

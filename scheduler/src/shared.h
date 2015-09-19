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

#ifndef SHARED_H_
#define SHARED_H_

	extern t_log* logger;
	extern char log_buffer[1024];

	typedef struct
	{
		char * name;
		void(*fn)();
		int args;
		char * description;
	} Command;

#endif

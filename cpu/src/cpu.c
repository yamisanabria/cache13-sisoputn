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

#include "config.h"
#include "connections.h"
#include "shared.h"

#include "filereader.h"
#include "cpumanager.h"


pthread_mutex_t mx_main;

/** Config info **/

t_log* logger;							//Declarado extern en shared.h
char log_buffer[1024];					//Declarado extern en shared.h


void initializeLogger(int argc, char* argv[]){
	int showLogInConsole = 0;
	if(argc > 0){
		int i = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "--show_log") == 0){
				showLogInConsole = 1;
				break;
			}
		}
	}
	logger = log_create("cpu.log", "CPU", showLogInConsole, LOG_LEVEL_INFO);
}

int main(int argc, char* argv[])
{
	initializeLogger(argc, argv);

	initFileConfig();
	
	// cpuConfig es global, ya no es necesario pasarselo
	setConnectionsParameters(cpuConfig);

	initializeRemoteFunctions();

	initCpuManager();


	int procesadores = config_get_int_value(cpuConfig, "CANTIDAD_HILOS");
	int i;
	for(i = 0; i < procesadores; i++){
		createCpu();
	}

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	config_destroy(cpuConfig);

	return EXIT_SUCCESS;
}

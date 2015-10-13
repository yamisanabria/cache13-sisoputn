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
#include <semaphore.h>

#include "shared.h"
#include "connections.h"
#include "console.h"
#include "pcb.h"
#include "cpu.h"
#include "ioqueue.h"
#include "pqueue.h"

pthread_mutex_t mx_main;

/** Config info **/
char* 				fileConfigDir = "config.cfg";	// Ruta a archivo config por defecto
t_config* 			schedulerConfig;				// Objeto de configuración de marta

t_log* 				logger;							//Declarado extern en shared.h
char 				log_buffer[1024];				//Declarado extern en shared.h
int					P_QUANTUM;						//Declarado extern en shared.h
char* 				P_METHOD;						//Declarado extern en shared.h

t_config* readFileConfig()
{
	t_config* config;
	char auxmsg[1024];
	config = config_create(fileConfigDir);
	char cwd[1024];

	if(!config){
		getcwd(cwd, sizeof(cwd));
        sprintf(auxmsg, "Archivo de configuracion inexistente: %s\nCurrent working dir: %s\n", fileConfigDir, cwd);
		log_error(logger, "Archivo de configuracion inexistente.");
		exit(1);
	}

	int configElements;
	configElements = config_keys_amount(config);

	if(configElements == 0){
		log_error(logger, "Archivo de configuración inválido.");
		exit(1);
	}

	if(!config_has_property(config, "PUERTO_ESCUCHA") ||
		!config_has_property(config, "ALGORITMO_PLANIFICACION")){
		log_error(logger, "Parámetros inválidos en archivo de configuración.");
		exit(1);
	} else {
		P_QUANTUM 	= config_get_int_value(config, "QUANTUM");
		P_METHOD	= config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	}

	return config;
}

void initializeLogger(int argc, char* argv[]){
	bool showLogInConsole = false;
	if(argc > 0){
		int i = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "--show_log") == 0){
				showLogInConsole = true;
				break;
			}
		}
	}
	logger = log_create("scheduler.log", "SCHEDULER", showLogInConsole, LOG_LEVEL_INFO);
}

int main(int argc, char* argv[])
{
	initializeLogger(argc, argv);

	schedulerConfig = readFileConfig();

	createCPUsList();

	setConnectionsParameters(schedulerConfig);

	listenStart();

	createPCB();

	createIOQueue();

	createProcessQueue();

	startConsole(); //Acá se queda en un while(1)

	//config_destroy(schedulerConfig);

	return EXIT_SUCCESS;
}

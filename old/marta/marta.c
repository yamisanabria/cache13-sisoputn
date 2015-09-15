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

#include "shared.h"
#include "connections.h"
#include "scheduler.h"
#include "planning.h"

pthread_mutex_t mx_main;

/** Config info **/
char* 	fileConfigDir = "config.cfg";	// Ruta a archivo config por defecto
t_config* martaConfig;					// Objeto de configuración de marta

t_log* logger;							//Declarado extern en shared.h
char log_buffer[1024];					//Declarado extern en shared.h

t_config* readFileConfig()
{
	t_config* config;
	config = config_create(fileConfigDir);

	int configElements;
	configElements = config_keys_amount(config);

	if(configElements == 0){
		log_error(logger, "Archivo de configuración inválido.");
		exit(1);
	}

	if(!config_has_property(config, "PUERTO_FS") ||
		!config_has_property(config, "IP_FS") ||
		!config_has_property(config, "PUERTO_LISTEN")){
		log_error(logger, "Parámetros inválidos en archivo de configuración.");
		exit(1);
	}

	return config;
}

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
	logger = log_create("marta.log", "MARTA", showLogInConsole, LOG_LEVEL_INFO);
}

int main(int argc, char* argv[])
{
	/* Inicializamos el logger */
	initializeLogger(argc, argv);
	/* Leemos config del archivo y validamos que esté bien */
	martaConfig = readFileConfig();
	/* Seteamos a las variables de conexión la info que tomamos del file .cfg */
	setConnectionsParameters(martaConfig);
	/* Ponemos a marta en escucha de jobs */
	initializeRemoteFunctions();
	/* Conectamos con el FS */
	connectFS();
	/* Levantamos el scheduler */
	setSchedulerParameters(martaConfig);
	startScheduler();
	/* Se queda esperando */
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	config_destroy(martaConfig);

	return EXIT_SUCCESS;
}

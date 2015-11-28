#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
#include <stdbool.h>
#include <utils.h>

#include "memory.h"
#include "connections.h"
#include "shared.h"

/** Config info **/
char* 	fileConfigDir = "config.cfg";	// Ruta a archivo config por defecto
t_config* memoryConfig;				    // Objeto de configuración de marta

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

	if(!config_has_property(config, "PUERTO_ESCUCHA") ||
		!config_has_property(config, "IP_SWAP") ||
		!config_has_property(config, "PUERTO_SWAP") ||
		!config_has_property(config, "MAXIMO_MARCOS_POR_PROCESO") ||
		!config_has_property(config, "CANTIDAD_MARCOS") ||
		!config_has_property(config, "TAMANIO_MARCOS") ||
		!config_has_property(config, "ENTRADAS_TLB") ||
		!config_has_property(config, "RETARDO_MEMORIA")){
		log_error(logger, "Parámetros inválidos en archivo de configuración.");
		exit(1);
	}

	return config;
}

void initializeLogger(int argc, char* argv[]){
	/*
	// showLogInConsole no se esta usando, por ello comento todo
	int showLogInConsole = 0;
	if(argc > 0){
		int i = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "--show_log") == 0){
				showLogInConsole = 1;
				break;
			}
		}
	}*/
	//logger = log_create("cpu.log", "CPU", showLogInConsole, LOG_LEVEL_INFO);
	logger = log_create("memory.log", "MEMORY", true, LOG_LEVEL_INFO);
}

//Escucha las 3 señales (se envian con kill -s SIG**** %pid%)
void listenSignal(int signal)
{
	write_start();
	//log_info(logger, "Comienzo sem");

	switch (signal)
	{
		case SIGUSR1:
			log_info(logger, "Signal SIGUSR1, limpiando TLB");
			clearTLB();
			break;
		case SIGUSR2:
			log_info(logger, "Signal SIGUSR2, limpiando memoria");
			clearMemory();
			break;
		case SIGPOLL:
			log_info(logger, "Signal SIGPOLL, imprimiendo dump de memoria");
			printMemory();
			break;
	}

	//log_info(logger, "fin sem");

	write_end();
}

int main(int argc, char* argv[])
{
	//Inicializacion variables de sincronizacion
	initSyn();

	//inicializo loger
	initializeLogger(argc, argv);

	//leo archivo de configuracion
	memoryConfig = readFileConfig();

	//Inicializo modulos
	initMemory(memoryConfig);
	initConnections(memoryConfig);

	//Me pongo a la escucha de CPUs
	listenStart();

	//Me conecto al Swap
	connectSwap();

	//Pongo a la escucha las 3 señales
	signal(SIGUSR1, listenSignal);
	signal(SIGUSR2, listenSignal);
	signal(SIGPOLL, listenSignal);

	// Imprimo cada 1 minuto la tasa de aciertos
	while(true)
	{
		sleep(60);
		printHitRate();
	}

	config_destroy(memoryConfig);

	return EXIT_SUCCESS;
}

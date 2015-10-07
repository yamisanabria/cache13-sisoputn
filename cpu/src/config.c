#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

char* 	fileConfigDir = "/home/eugenio/tp-2015-2c-Aprobados/cpu/config.cfg";	// Ruta a archivo config por defecto
t_config* cpuConfig;				// Objeto de configuración de marta

t_log* logger;							//Declarado extern en shared.h
char log_buffer[1024];					//Declarado extern en shared.h


void initFileConfig()
{
	logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	//t_config* config;
	cpuConfig = config_create(fileConfigDir);

	int configElements;
	configElements = config_keys_amount(cpuConfig);

	if(configElements == 0){
		log_error(logger, "Archivo de configuración inválido.");
		exit(1);
	}

	if(!config_has_property(cpuConfig, "IP_PLANIFICADOR") ||
		!config_has_property(cpuConfig, "PUERTO_PLANIFICADOR") ||
		!config_has_property(cpuConfig, "IP_MEMORIA") ||
		!config_has_property(cpuConfig, "PUERTO_MEMORIA") ||
		!config_has_property(cpuConfig, "CANTIDAD_HILOS") ||
		!config_has_property(cpuConfig, "RETARDO")){
		log_error(logger, "Parámetros inválidos en archivo de configuración.");
		exit(1);
	}


}


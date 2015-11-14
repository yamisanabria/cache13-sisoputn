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
		// los datos son unsigned long long por que son nanosegundos
		// debido a que que probablemente tengamos que cambiar los sleep
		// de la cpu a usleep para acelerar los tests
		unsigned long long init; // timestamp de cuando inicio uso de cpu
		unsigned long long end; // timestamp de cuando termino uso de cpu
	} QUANTUMstat;

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
		// guardo un struct con el dato de
		// cuando arranco a ejecutar
		// y cuando temino de ejecutar
		// para luego procesarlo y sacar porcentage de uso en
		// ultimo minuto
		t_list* rawstats; // lista con datos QUANTUMstat
	} CPU;
#endif

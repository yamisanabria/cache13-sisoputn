#include "shared.h"
#include "pcb.h"

void cmd_run(char ** args)
{
	char* path = string_duplicate(args[1]);
	printf("\nEjecutar nuevo proceso de ruta (%s)\n", path);
	if(!runNewProcess(path)){
		printf("Ha ocurrido un error al correr el proceso, revise el log.\n");
	}

	free(path);
}

void cmd_end(char ** args)
{
	char* pid = string_duplicate(args[1]);
	printf("\nEND %s called\n", pid);
	free(pid);
}

void cmd_ps()
{
	printf("\nPS called\n");
}

void cmd_cpu()
{
	printf("\nCPU called\n");
}

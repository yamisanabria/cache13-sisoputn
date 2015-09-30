#include "shared.h"
#include "pcb.h"

PCBItem* _item;
void __printpcb();

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

void __printpcb(void* item) {
	printf('print datos del pcb');
	// todo, debbuguear estas lineas por que me tiran seg fault
	//_item = item;
	//printf("%d,%d,%s,%d,%s",_item->PID, _item->counter, _item->path, _item->status, _item->start);
}
/* TODO si se pasa de argumento el id estaria bueno mostrar ese solo*/
void cmd_ps()
{
	printf("\n********************************\n");
	printf("**   Listado de procesos\n");
	printf("********************************\n");
	printf("pid,counter,status,start,path\n\n");
	
	
	list_iterate(getPCBlist(), &__printpcb);
}

void cmd_cpu()
{
	// TODO
	printf("\nCPU called\n");
}


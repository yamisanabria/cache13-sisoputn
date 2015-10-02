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

/* TODO si se pasa de argumento el id estaria bueno mostrar ese solo*/
void cmd_ps()
{
	printf("\n********************************\n");
	printf("**   Listado de procesos\n");
	printf("********************************\n");
	printf("\n\n");
	
	
	void __printpcb(PCBItem* item) {
		char* _status[6] = {"NEW", "READY", "RUNNING", "BLOCKED", "FAILED", "FINISHED"};
		printf("PID: %d\ncounter: %d\nfile: %s\nstatus:%s\n**********\n" ,item->PID, item->counter, item->path, _status[item->status]);

		//printf("%d,%d,%s,%d,%s",item->PID, item->counter, item->path, item->status, item->start);
	}

	list_iterate(getPCBlist(), (void*)__printpcb);
}

void cmd_cpu()
{
	// TODO
	printf("\nCPU called\n");
}


#include "shared.h"

void cmd_run(char ** args)
{
	char* path = string_duplicate(args[1]);
	printf("\nRUN %s called\n", path);
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

#include "shared.h"
#include "commands.h"

Command* 	cmd;
t_list* 	commands;

char* 		input;				// Utilizada para el ingreso del usuario
size_t 		inputLength = 100;	// Length maximo de entrada en el ingreso del usuario

// Crea un nuevo comando
Command * newCommand(char * name, void (*fn)(), int args, char * description)
{
	cmd = malloc(sizeof(Command));
	cmd->name = name;
	cmd->fn = fn;
	cmd->args = args;
	cmd->description = description;
	return cmd;
}

// Carga de lista de comandos
void loadCommands()
{
	commands = list_create();
	list_add(commands, newCommand("correr", &cmd_run	, 1, "(PATH)~$: Ejecuta el programa de la ruta señalada."));
	list_add(commands, newCommand("finalizar", &cmd_end	, 1, "(PID)~$: Coloca el puntero en la última línea del programa."));
	list_add(commands, newCommand("ps", &cmd_ps			, 0, "()~$: Imprime una lista con los estados de los programas."));
	list_add(commands, newCommand("cpu", &cmd_cpu		, 0, "()~$: Imprime el uso de cada CPU."));
}

// Imprime menu de comandos
void printMenu()
{
	int i;

	printf("\n\n#########################################################################\n");
	printf("#########################################################################\n");
	printf("Listado de comandos disponibles:\n");
	for(i = 0; i < list_size(commands); i++)
	{
		cmd = list_get(commands, i);
		printf("   %-20s (%d)   %s\n", cmd->name, cmd->args, cmd->description);
	}
	puts("\nIngrese comando:");
}

// Valida y ejecuta comando especifico
void executeCommand(char * c)
{
	c[strlen(c)-1] = '\0'; //quito /n final

	if(string_is_empty(c) || c[0] == ' ' || c[strlen(c)-1] == ' ')
	{
		printf("Comando inexistente.\n");
		return;
	}

	char ** args = string_split(c, " ");
	char * name = args[0];

	int i;
	for(i = 0; i < name[i]; i++)
		name[i] = tolower(name[i]);

	int _get(Command * cc)
	{return strcmp(cc->name, name) == 0;}

	cmd = list_find(commands,(void*) _get);

	if(cmd == NULL)
		printf("Comando inexistente.\n");
	else
	{
		if(charArray_length(args) - 1 == cmd->args)
			cmd->fn(args);
		else
			printf("La cantidad de parametros no coincide [%d/%d].\n", charArray_length(args) - 1, cmd->args);
	}

	freeCharArray(args);
}

void startConsole(){

	loadCommands();
	printMenu();

	while(1){
		getline (&input, &inputLength, stdin);
		if(input[0] != '\n'){
			executeCommand(input);
			printf("\nIngrese comando o enter para ver el menú:\n");
		}
		else{
			printMenu();
		}
	}
}

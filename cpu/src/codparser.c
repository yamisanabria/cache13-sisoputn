#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "shared.h"
#include "codinstructions.h"


bool consumeQuantums(CPU* cpu){

	// TODO !! interpretar cod

	return false;
}







/*
Instruction* 	ins;
t_list* 	instructions;

char* 		input;				// Utilizada para el ingreso del usuario
size_t 		inputLength = 100;	// Length maximo de entrada en el ingreso del usuario

// Crea un nuevo comando
Instruction * newCommand(char * name, void (*fn)(), int args, char * description)
{
	ins = malloc(sizeof(Instruction));
	ins->name = name;
	ins->fn = fn;
	ins->args = args;
	ins->description = description;
	return ins;
}

// Carga de lista de instrucciones cod
void loadInstructions()
{
	instructions = list_create();
	list_add(instructions, newCommand("correr", &ins_run	, 1, "(PATH)~$: Ejecuta el programa de la ruta señalada."));
	list_add(instructions, newCommand("finalizar", &ins_end	, 1, "(PID)~$: Coloca el puntero en la última línea del programa."));
	list_add(instructions, newCommand("ps", &ins_ps			, 0, "()~$: Imprime una lista con los estados de los programas."));
	list_add(instructions, newCommand("cpu", &ins_cpu		, 0, "()~$: Imprime el uso de cada CPU."));
}

// Valida y ejecuta comando especifico
void executeCommand(CPU* cpu, char* c)
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

	ins = list_find(instructions,(void*) _get);

	if(ins == NULL) {
		printf("Comando inexistente.\n");
	} else if(charArray_length(args) - 1 != ins->args) {
		printf("La cantidad de parametros no coincide [%d/%d].\n", charArray_length(args) - 1, ins->args);
	} else {
		ins->fn(cpu, args);
	}

	freeCharArray(args);
}


//			executeCommand(input);
	
*/
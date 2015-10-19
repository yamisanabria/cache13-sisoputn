#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <commons/string.h>
#include <string.h>

#include "shared.h"
#include "codparser.h"
#include "codinstructions.h"
#include "filereader.h"
#include "utils.h"


typedef struct
{
	char * name;
	void (*fn)();
	int args;
} Instruction;

t_list* 	instructions;


// retardo por defecto
int RETARDO = 1;

void setCpuWait(int _retardo){
	RETARDO = _retardo;
}

void initInstructions(){
	// Crea Instruccion
	Instruction * newInstruction(char * name, void (*fn)(), int args){
		Instruction* ins = malloc(sizeof(Instruction));
		ins->name = name;
		ins->fn = fn;
		ins->args = args;
		return ins;
	}

	instructions = list_create();
	list_add(instructions, newInstruction("iniciar", &ins_iniciar, 1));
	list_add(instructions, newInstruction("leer", &ins_leer, 1));
	list_add(instructions, newInstruction("escribir", &ins_escribir, 2));
	list_add(instructions, newInstruction("entrada-salida", &ins_entradaSalida, 1));
	list_add(instructions, newInstruction("finalizar", &ins_finalizar, 0));
}

void consumeQuantum(CPU* cpu){
	if(cpu->quantum == 0){
		endQuantum(cpu);
		return;
	}

	// sleep de retardo
	sleep(RETARDO);

	// el -1 es por que el process_counter va de 1 a n lineas
	// y el get_nth_line de 0 a n-1 lineas
	char *line = get_nth_line(cpu->codfile, (cpu->process_counter - 1));

	cpu->quantum = cpu->quantum - 1;
	cpu->process_counter = cpu->process_counter + 1;

	runLine(line, cpu);

	free(line);
}

/*
 * Informa al scheduler el resultado del programa al finalizar
 * el quantum o el programa
 */
void endQuantum(CPU* cpu){
	// manda al scheduler todo el cpu->execResponseBuffer
	// TODO

	//@pablovergne: Tendrías que fijarte si terminó de ejecutar el archivo entero
	// Si es así me mandás un parámetro diferente, mirate el protocolo, en scheduler
}

void runLine(char* line, CPU* cpu) {
	// la copio por que uso el string original si ocurre un fallo
	char *_line = string_duplicate(line);

	/* @pablovergne: TE COMENTO ESTO PORQUE ME SACA EL ÚLTIMO CARACTER DE UNA LÍNEA */
	//_line[strlen(_line)-1] = '\0'; //quito /n final

	// ver de usar string_n_split y string_starts_with;
	//should_bool(string_starts_with("MiArchivo.txt", "txt")) be truthy;
	char ** args = string_split(_line, " ");
	char * name = args[0];

	bool _get(Instruction* cc){
		if(cc == NULL) {
			return false;
		}
		return strcmp(cc->name, name) == 0;
	}

	Instruction* instruction = list_find(instructions, (void*) _get);

	if(instruction == NULL) {
		printf("Instruccion inexistente:\n%s\n", line);
		exit(EXIT_FAILURE);
	} else if(charArray_length(args) - 1 != instruction->args) {
		printf("La cantidad de parametros no coincide [%d/%d].\n%s\n", charArray_length(args) - 1, instruction->args, line);
		exit(EXIT_FAILURE);
	}

	instruction->fn(cpu, args);
	freeCharArray(args);
	free(_line);
}

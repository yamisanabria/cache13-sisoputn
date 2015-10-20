#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <socket.h>

#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include <string.h>

#include "shared.h"
#include "codparser.h"
#include "codinstructions.h"
#include "filereader.h"
#include "utils.h"


typedef struct
{
	//char * name;
	void (*fn)();
	int args;
} Instruction;

t_dictionary* 	instructions;

// retardo por defecto
int RETARDO = 1;

void setCpuWait(int _retardo){
	RETARDO = _retardo;
}

void initInstructions(){
	// Crea Instruccion
	Instruction * newInstruction(/*char * name, */void (*fn)(), int args){
		Instruction* ins = malloc(sizeof(Instruction));
		ins->fn = fn;
		ins->args = args;
		return ins;
	}

	instructions = dictionary_create();
	dictionary_put(instructions, "iniciar", newInstruction(&ins_iniciar, 1));
	dictionary_put(instructions, "leer", newInstruction(&ins_leer, 1));
	dictionary_put(instructions, "escribir", newInstruction(&ins_escribir, 2));
	dictionary_put(instructions, "entrada-salida", newInstruction(&ins_entradaSalida, 1));
	dictionary_put(instructions, "finalizar", newInstruction(&ins_finalizar, 0));
}

void consumeQuantum(CPU* cpu){
	if(cpu->quantum == 0){
		runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, string_itoa(cpu->execPid), "1", string_itoa(cpu->process_counter + 1), cpu->execResponseBuffer, "");
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


void runLine(char* line, CPU* cpu) {
	// la copio por que uso el string original si ocurre un fallo
	char *_line = string_duplicate(line);

	// ver de usar string_n_split y string_starts_with;
	//should_bool(string_starts_with("MiArchivo.txt", "txt")) be truthy;
	char ** args = string_split(_line, " ");

	Instruction* instruction = dictionary_get(instructions, args[0]);

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

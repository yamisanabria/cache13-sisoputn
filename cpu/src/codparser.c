#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <socket.h>
#include <sys/time.h>


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

void updateCpuTimer(CPU* cpu){

	unsigned long long now = getTimeNow();

	bool _hacemasDeUnMinuto(QUANTUMstat *s) {
		return s->end && s->end < (now - (unsigned long long)(60*1000));
	}
	list_remove_and_destroy_by_condition(cpu->rawstats, (void*) _hacemasDeUnMinuto, free);

	void _updateEndTime(QUANTUMstat* st){
		if(!st->end)
			st->end = now;
	}
	list_iterate(cpu->rawstats, (void*) _updateEndTime);
}

void consumeQuantum(CPU* cpu){
	if(cpu->quantum == 0){
		char* pid = string_itoa(cpu->execPid);
		char* pc = string_itoa(cpu->process_counter);
		runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, pid, "1", pc, cpu->execResponseBuffer, "");
		free(pid);
		free(pc);
		return;
	}

	/** inicio de quantum, guardo estadisticas */
	QUANTUMstat* _stats = malloc(sizeof(QUANTUMstat));

	// guardo tiempo de inicio
	_stats->init = getTimeNow();
	_stats->end = 0;
	list_add(cpu->rawstats, _stats);
	/** fin de inicio de quantum */

	// sleep de retardo
	usleep(RETARDO);

	char *line;
	if(cpu->process_counter == -1){ //Forzaron finalizar
		line = string_duplicate("finalizar");
	} else {
		// el -1 es por que el process_counter va de 1 a n lineas
		// y el get_nth_line de 0 a n-1 lineas
		line = get_nth_line(cpu->codfile, (cpu->process_counter - 1));

		cpu->quantum = cpu->quantum - 1;
		//cpu->process_counter = cpu->process_counter + 1; Esto lo sacamos de acá en el runLine, porque tiene que saber qué línea ejecuta para ver si suma o no
	}

	sprintf(log_buffer, "Ejecutando instrucción (%s) del proceso %d", line, cpu->execPid);
	log_info(logger, log_buffer);

	runLine(line, cpu);

	free(line);
}


void runLine(char* line, CPU* cpu) {
	// la copio por que uso el string original si ocurre un fallo
	char *_line = string_duplicate(line);

	char ** instr = string_n_split(_line, 2, " ");

	Instruction* instruction = dictionary_get(instructions, instr[0]);

	char ** args = string_n_split(_line, instruction->args + 1, " ");

	if(instruction == NULL) {
		printf("Instruccion inexistente:\n%s\n", line);
		exit(EXIT_FAILURE);
	} else if(charArray_length(args) - 1 != instruction->args) {
		printf("La cantidad de parametros no coincide [%d/%d].\n%s\n", charArray_length(args) - 1, instruction->args, line);
		exit(EXIT_FAILURE);
	}

	instruction->fn(cpu, args);
	freeCharArray(args);
	freeCharArray(instr);
	free(_line);
}

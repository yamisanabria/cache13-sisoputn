#include <stdlib.h>
#include <string.h>

#include <commons/string.h>
#include <socket.h>

#include "shared.h"
#include "codparser.h"

void ins_iniciar(CPU* cpu,char ** args) {
	char* _pages = string_duplicate(args[1]);
	char* pid = string_itoa(cpu->execPid);
	runFunction(cpu->socketIdMemory, "cpu_mem_startProcess", 2, pid, _pages);
	free(_pages);
	free(pid);
}

void ins_leer(CPU* cpu,char ** args) {
	char* _page = string_duplicate(args[1]);
	char* pid = string_itoa(cpu->execPid);
	runFunction(cpu->socketIdMemory, "cpu_mem_read", 2, pid, _page);
	free(_page);
	free(pid);
}

void ins_escribir(CPU* cpu,char ** args) {
	char* _page = string_duplicate(args[1]);
	// char* _data = string_duplicate(args[2]);
	// La data está con comillas, se las sacamos
	char* _data = string_substring(args[2], 1, string_length(args[2]) - 2);

	char* pid = string_itoa(cpu->execPid);
	runFunction(cpu->socketIdMemory, "cpu_mem_write", 3, pid, _page, _data);

	// estaria mejor que esto se haga cuando responda el adm de memoria, pero como cuando
	// responde no manda los parametros que necesito (pagina y contenido escrito) lo hago
	// aca
	char* _buffer = string_from_format("mProc %s - Pagina %s escrita: %s\n", pid, _page, _data);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);
	free(pid);
	free(_page);
	free(_data);
}

void ins_entradaSalida(CPU* cpu,char ** args) {
	char* pid = string_itoa(cpu->execPid);
	char* pc = string_itoa(cpu->process_counter);
	char* _sleep = string_duplicate(args[1]);

	char* _buffer = string_from_format("mProc %s en entrada-salida de tiempo %s\n", pid, _sleep);
	string_append(&cpu->execResponseBuffer, _buffer);

	/* MENSAJE A SCHEDULER:
	cpu_sc_process_back(pid, status, messages, data)
	donde:
		pid = id del proceso
		status = 4: El proceso pasó a estar bloqueado por E/S
		messages = mensajes que se tienen que imprimir
		data = info relacionada al estado. Por ejemplo si entra en E/S, sería el tiempo que tiene que bloquearse.
	*/

	updateCpuTimer(cpu);
	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 5, pid, "4", pc, cpu->execResponseBuffer, _sleep);

	free(_sleep);
	free(pc);
	free(pid);
	free(_buffer);

}

void ins_finalizar(CPU* cpu) {
	char* pid = string_itoa(cpu->execPid);
	char* _buffer = string_from_format("mProc %s finalizado\n", pid);
	char* pc = string_itoa(cpu->process_counter);
	string_append(&cpu->execResponseBuffer, _buffer);
	free(_buffer);

	updateCpuTimer(cpu);
	// le aviso a la memoria que termino proceso
	runFunction(cpu->socketIdMemory, "cpu_mem_endProcess", 1, pid);
	// le aviso al scheduler que termino proceso
	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, pid, "3", pc, cpu->execResponseBuffer);

	cpu->quantum = 0;

	free(pid);
	free(pc);
}

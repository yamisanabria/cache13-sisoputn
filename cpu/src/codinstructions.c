#include <stdlib.h>
#include <string.h>

#include <commons/string.h>
#include <socket.h>

#include "shared.h"

/**
 *  Agrega el string str al final del execbuffer del cpu
 */
void addResponseToExecbuffer(CPU* cpu, char *str){
	/*
	Avoid using strcat in C code. The cleanest and, most importantly, the safest way is to use snprintf:

	char buf[256];
	snprintf(buf, sizeof buf, "%s%s%s%s", str1, str2, str3, str4);
	Some commenters raised an issue that the number of arguments may not match the format string and the 
	code will still compile, but most compilers already issue a warning if this is the case.
	*/

	snprintf(cpu->execResponseBuffer, strlen(cpu->execResponseBuffer) + strlen(str) + 1, "%s%s", cpu->execResponseBuffer, str);

}

void ins_iniciar(CPU* cpu,char ** args) {
	// TODO testear

	char* _pages = string_duplicate(args[1]);
	runFunction(cpu->socketIdMemory, "cpu_mem_startProcess", 2, cpu->execPid, _pages);
	free(_pages);
}

void ins_leer(CPU* cpu,char ** args) {
	// TODO testear

	char* _page = string_duplicate(args[1]);
	runFunction(cpu->socketIdMemory, "cpu_mem_read", 2, cpu->execPid, _page);
	free(_page);
}

void ins_escribir(CPU* cpu,char ** args) {
	// TODO testear

	char* _page = string_duplicate(args[1]);
	char* _data = string_duplicate(args[2]);
	runFunction(cpu->socketIdMemory, "cpu_mem_read", 3, cpu->execPid, _page, _data);

	// estaria mejor que esto se haga cuando responda el adm de memoria, pero como cuando
	// responde no manda los parametros que necesito (pagina y contenido escrito) lo hago
	// aca
	char* _buffer = string_from_format("mProc %s - Pagina %s escrita: %s", cpu->execPid, _page, _data);
	addResponseToExecbuffer(cpu, _buffer);
	free(_buffer);

	free(_page);
	free(_data);
}

void ins_entradaSalida(CPU* cpu,char ** args) {
	// TODO testear

	char* _sleep = string_duplicate(args[1]);
	
	char* _buffer = string_from_format("mProc %s en entrada-salida de tiempo %s\n", cpu->execPid, _sleep);
	addResponseToExecbuffer(cpu, _buffer);
	
	/* MENSAJE A SCHEDULER:
	cpu_sc_process_back(pid, status, messages, data)
	donde:
		pid = id del proceso
		status = 4: El proceso pasó a estar bloqueado por E/S
		messages = mensajes que se tienen que imprimir
		data = info relacionada al estado. Por ejemplo si entra en E/S, sería el tiempo que tiene que bloquearse.
	*/

	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, cpu->execPid, "4", cpu->execResponseBuffer, _sleep);

	free(_sleep);
	free(_buffer);

}

void ins_finalizar(CPU* cpu) {
	// TODO testear
	
	char* _buffer = string_from_format("mProc %s finalizado\n", cpu->execPid);
	addResponseToExecbuffer(cpu, _buffer);
	free(_buffer);
	// le aviso a la memoria que termino proceso
	runFunction(cpu->socketIdMemory, "cpu_mem_endProcess", 1, cpu->execPid);
	// le aviso al scheduler que termino proceso
	runFunction(cpu->socketIdScheduler, "cpu_sc_process_back", 4, cpu->execPid, "4", cpu->execResponseBuffer, "");

}

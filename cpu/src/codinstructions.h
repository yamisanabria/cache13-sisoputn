#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

void addResponseToExecbuffer(CPU* cpu, char *str);

void ins_iniciar(CPU* cpu,char ** args);

void ins_leer(CPU* cpu,char ** args);

void ins_escribir(CPU* cpu,char ** args);

void ins_entradaSalida(CPU* cpu,char ** args);

void ins_finalizar(CPU* cpu);

#endif

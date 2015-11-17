#ifndef CODPARSER_H_
#define CODPARSER_H_
	
	void consumeQuantum(CPU* cpu);
	void endQuantum(CPU* cpu);
	void runLine(char* line, CPU* cpu);
	void setCpuWait(int _retardo);
	void initInstructions();
	void updateCpuTimer(CPU* cpu);

#endif

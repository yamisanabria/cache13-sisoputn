#ifndef CONSOLE_H_
#define CONSOLE_H_

	// Crea un nuevo commando
	Command * newCommand(char * name, void (*fn)(), int args, char * description);

	// Carga de lista de comandos
	void loadCommands();

	// Imprime menu de comandos
	void printMenu();

	// Valida y ejecuta comando especifico
	void executeCommand(char * c);

	void startConsole();
#endif

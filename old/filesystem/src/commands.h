#ifndef FILESYSTEM2_COMMANDS_H_
#define FILESYSTEM2_COMMANDS_H_


	typedef struct
	{
		char * name;
		void(*fn)();
		int args;
		char * description;
	} command;

	void initSyn_commands();

	// Guarda el bloque descargado
	void downloadedBlock(char * block);

	// Agrega un archivo al MDFS
	bool fileAdd(char * localFilePath, char * dirPath, bool check);

	// Muestra el estado actual de la capacidad del sistema
	void showSpace();

	// Imprime el MDFS
	void cmd_print(char ** args);

	// Agrega un directorio en una ruta
	void cmd_directory_add(char ** args);

	// Cambia el nombre de un directorio
	void cmd_directory_rename(char ** args);

	// Mueve un directorio de un directorio a otro
	void cmd_directory_move(char ** args);

	void cmd_directory_delete(char ** args);

	void cmd_file_add(char ** args);

	void cmd_file_rename(char ** args);

	void cmd_file_move(char ** args);

	void cmd_file_delete(char ** args);

	void cmd_file_download(char ** args);

	void cmd_file_md5(char ** args);

	void cmd_block_show(char ** args);

	void cmd_block_delete(char ** args);

	void cmd_block_copy(char ** args);

	void cmd_format(char ** args);

	void cmd_nodo_add(char ** args);

	void cmd_nodo_delete(char ** args);

	void cmd_nodo_print(char ** args);

	// Crea un nuevo commando
	command * newCommand(char * name, void (*fn)(), int args, char * description);

	// Carga de lista de comandos
	void loadCommands();

	// Imprime menu de comandos
	void printMenu();

	// Valida y ejecuta comando especifico
	void executeCommand(char * c);


#endif

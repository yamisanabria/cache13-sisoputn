#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

	void initConnections(t_config* memoryConfig);

	//-###############################################################################################-//
	//-###-[GENERAL]-#################################################################################-//
	//-###############################################################################################-//

	// CPU se desconecta
	void cpuDisconnected(socket_connection* socketInfo);

	// CPU se conecta
	void cpuNew(socket_connection* socketInfo);

	// Comeinza escucha de CPUs
	void listenStart();

	// Swap se desconecta
	void swDisconnected(socket_connection* socketInfo);

	// Se conecta al swap
	void connectSwap();

	//-###############################################################################################-//
	//-###-[INPUT]-###################################################################################-//
	//-###############################################################################################-//

	//-###########################################-//
	//-###-[CPU]-#################################-//
	//-###########################################-//

	// Nos indica que comienza un proceso
	void cpu_startProcess(socket_connection * connection, char ** args);

	// Nos solicita la lectura de un frame
	void cpu_read(socket_connection * connection, char ** args);

	// Nos indica escribir un frame
	void cpu_write(socket_connection * connection, char ** args);

	// Nos indica que finalizo un proceso
	void cpu_endProcess(socket_connection * connection, char ** args);

	//-###########################################-//
	//-###-[SWAP]-################################-//
	//-###########################################-//

	// Nos indica que comenzo correctamente el proceso
	void sw_startProcessOk(socket_connection * connection, char ** args);

	// Nos indica que no tiene suficiente espacio pra el proceso a comenzar
	void sw_noSpace(socket_connection * connection, char ** args);

	// Nos devuelve el conenido de una pagina
	void sw_page(socket_connection * connection, char ** args);

	//-###############################################################################################-//
	//-###-[OUTPUT]-##################################################################################-//
	//-###############################################################################################-//

	//-###########################################-//
	//-###-[CPU]-#################################-//
	//-###########################################-//

	// Indicamos que se comenzo correctamente el proceso
	void cpu_startProcessOk(int socket, int pid);

	// Indicamos que no hay frames suficientes para comenzar un proceso
	void cpu_noFrames(int socket, int pid);

	// Indicamos que no hay suficiente espacio para comenzar un proceso
	void cpu_noSpace(int socket, int pid);

	// Enviamos contenido de un frame
	void cpu_frameData(int socket, int frame, char * data);

	// Indicamos que se realizo la escritura
	void cpu_writeOk(int socket);


	//-###########################################-//
	//-###-[SWAP]-################################-//
	//-###########################################-//

	//Le indicamos que comienza un proceso con la necesidad de páginas
	void sw_startProcess(int pid, int pages);

	//Solicitamos una pagina
	void sw_getPage(int pid, int page);

	//Escribimos una pagina
	void sw_setPage(int pid, int page, char *data);

	//Hacemos intercambio de paginas
	void sw_swapping(int pid, int setPage, char * data, int getPage);

	//Indicamos la finalizacion de un proceso
	void sw_endProcess(int pid);

#endif

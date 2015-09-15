#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

	/*** FUNCIONES COMO SERVIDOR ***/

	void scDisconnected(int socket);

	void memDisconnected(int socket);

	/*** FUNCIONES COMO CLIENTE ***/
	void connectScheduler();

	void connectMemory();

	/*** FUNCIONES INTERNAS ***/

	/**
	* @NAME: setConnectionsParameters
	* @DESC: Seteamos en variables los datos de conexión a los procesos leídos del archivo .cfg
	*/
	void setConnectionsParameters(t_config* martaConfig);

	void initializeRemoteFunctions();

#endif

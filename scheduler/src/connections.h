#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

	/*** FUNCIONES COMO SERVIDOR ***/

	void cpuNew(int socket, char * ip);

	void cpuDisconnected(int socket);


	/*** FUNCIONES COMO CLIENTE ***/


	/*** FUNCIONES INTERNAS ***/

	/**
	* @NAME: setConnectionsParameters
	* @DESC: Seteamos en variables los datos de conexión a los procesos leídos del archivo .cfg
	*/
	void setConnectionsParameters(t_config* martaConfig);

	void listenStart();

	void initializeRemoteFunctions();

#endif

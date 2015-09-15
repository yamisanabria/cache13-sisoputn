#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_

	/*** FUNCIONES COMO SERVIDOR ***/

	/**
	* @NAME: jobStarts
	* @DESC: Un job le pide a marta comenzar una nueva tarea.
	* 	- args[0] files: archivos a procesar (char* separados por comas)
    *	- args[1] combiner: determina si el job soporta combiner (1=SI, 0=NO)
	*/
	void jobStarts(int socket, char ** args);
	/**
	* @NAME: jobNew
	* @DESC: Se conecta un nuevo job
	*/
	void jobNew(int socket, char * ip);
	/**
	* @NAME: jobDisconnected
	* @DESC: Un job se nos desconecta
	*/
	void jobDisconnected(int socket);
	/**
	* @NAME: fsDisconnected
	* @DESC: El FS se nos desconecta
	*/
	void fsDisconnected(int socket);
	/**
	* @NAME: fsNewFilesInfo
	* @DESC: Un job le pide a marta comenzar una nueva tarea.
	* 	- args[0] filesInfo: estructura de datos serializada con los bloques y nodos de cada archivo solicitado (char*)
	*/
	void fsNewFilesInfo(int socket, char ** args);



	/*** FUNCIONES COMO CLIENTE ***/
	/**
	* @NAME: fsGetFilesInfo
	* @DESC: Le pedimos al filesystem info de nodos y bloques sobre archivos de tareas que tenemos que planificar
	* 	- filesNames: archivos de tareas a procesar (char* separados por comas)
	*/
	void fsGetFilesInfo(char* filesNames);
	/**
	* @NAME: jobRunMap
	* @DESC: Le pedimos al job que ejecute map
	*/
	void jobRunMap(MartaTask* task, Node* node, Routine* routine);
	/**
	* @NAME: jobRunReduce
	* @DESC: Le pedimos al job que ejecute reduce
	*/
	void jobRunReduce(MartaTask* task, Node* node, Routine* routine);
	/**
	* @NAME: jobEnds
	* @DESC: Job finaliza mal o bien
	*/
	void jobEnds(MartaTask* task, char* result, char* errorMsg);
	/**
	* @NAME: fsCopyResultFile
	* @DESC: Le decimos al FS que copie el resultado del file en el MDFS
	*/
	void fsCopyResultFile(Routine* routine);

	/*** FUNCIONES INTERNAS ***/

	/**
	* @NAME: setConnectionsParameters
	* @DESC: Seteamos en variables los datos de conexión a los procesos leídos del archivo .cfg
	*/
	void setConnectionsParameters(t_config* martaConfig);
	/**
	* @NAME: connectFS
	* @DESC: Se conecta al FS
	*/
	void connectFS();
	/**
	* @NAME: listenStart
	* @DESC: Se pone en escucha de jobs
	*/
	void listenStart();

	void initializeRemoteFunctions();

#endif

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

	/**
	* @NAME: queueNewTask
	* @DESC: Encola una nueva tarea que un job nos dijo para planear y la marca como NEW
	* 	Devuelve el id generado para la tarea
	*/
	char* queueNewTask(MartaTask* task);
	/**
	* @NAME: createNewIdForTask
	* @DESC: Devuelve un id nuevo para que sea asignado a una tarea
	*/
	int createNewIdForTask();
	/**
	* @NAME: startScheduler
	* @DESC: Levanta el planificador
	*/
	void startScheduler();
	/**
	* @NAME: setSchedulerParameters
	* @DESC: Setea parámetros varios para el planificador
	*/
	void setSchedulerParameters(t_config* martaConfig);
	/**
	* @NAME: getTotalRunningTasks
	* @DESC: Dice cuantas tareas corriendo hay actualmente
	*/
	int getTotalRunningTasks();
	/**
	* @NAME: getTotalReadyTasks
	* @DESC: Dice cuantas tareas listas para ejecutar hay actualmente
	*/
	int getTotalReadyTasks();
	/**
	* @NAME: getTotalNewTasks
	* @DESC: Dice cuantas tareas nuevas hay actualmente
	*/
	int getTotalNewTasks();
	/**
	* @NAME: getTaskToStart
	* @DESC: Devuelve una tarea de la lista de ready para empezarla
	*/
	MartaTask* getTaskToStart();
	/**
	* @NAME: startTask
	* @DESC: Comienza la tarea
	*/
	void startTask(MartaTask* task);
	/**
	* @NAME: readyTask
	* @DESC: Marca una tarea como lista para ejecutar
	*/
	void readyTask(MartaTask* task);
	/**
	* @NAME: readyTask
	* @DESC: Mata una tarea
	*/
	void killTask(MartaTask* task, char* errorMsg);
	/**
	* @NAME: endTask
	* @DESC: Tarea finaliza correctamente
	*/
	void endTask(MartaTask* task);

#endif

#ifndef SHARED_H_
#define SHARED_H_

	#include <semaphore.h>

	extern t_log* logger; //Logger de marta
	extern char log_buffer[1024];

	typedef enum {MTS_NEW, MTS_READY, MTS_RUNNING, MTS_SUCCESSFUL, MTS_FAILED} MartaTaskStatus;
	/* Estructura para representar una tarea que un job pide */
	typedef struct MartaTask
	{
		char* id;				//ID de la tarea
		pthread_t thread;
		char* job_id;			//ID del job
		socket_connection* socket;	//Socket con el job
		MartaTaskStatus status;	//Estado de la tarea
		t_list* filesNames;		//Lista de nombres de los archivos
		char* filesNamesRaw;	//Nombres de archivos separados por coma
		int usingCombiner;  	//Si usa combiner (1 ó 0)
		t_list* routines;		//Lista de rutinas que dependen de la tarea
		t_list* nodes;			//Nodos que participan de la tarea
		char* resultFile;		//Donde va el resultado
	} MartaTask;

	typedef enum {NS_ONLINE, NS_OFFLINE} NodeStatus;
	/* Estructura para representar un nodo */
	typedef struct Node
	{
		char* id;				//ID del nodo
		t_list* routines;		//Lista de rutinas del nodo sobre las tareas
		int weight;				//Indica la cantidad de laburo que está haciendo
		NodeStatus status;		//El estado del nodo
		char* ip;				//IP del nodo
		char* port;				//Puerto del nodo
		sem_t  mutex;
		pthread_t thread;
	} Node;

	typedef struct Block
	{
		char* id;		//ID del bloque
		char* node_id;	//Nodo al que pertenece
		char* file;		//Archivo del que depende
		char* number;	//Númeor de bloque
		char* node_number;	//Númeor de bloque en nodo
	} Block;

	typedef enum {RS_NEW, RS_RUNNING, RS_SUCCESSFUL, RS_FAILED} RoutineStatus;
	typedef enum {RT_MAP, RT_PARTIAL_REDUCE, RT_REDUCE} RoutineType;
	/* Estructura para representar una rutina de un nodo */
	typedef struct Routine
	{
		char* id;				//ID de la rutina
		RoutineType type;		//Lista de rutinas de las tareas
		RoutineStatus status;	//El estado del nodo
		Block* block;			//Bloque del eventual MAP
		t_list* files;			//archivos del eventual REDUCE tipo FileToReduce
		char* resultFile;		//Nombre del archivo resultado
		MartaTask* task;		//Tarea a la que pertenece
		int weight;				//Cuanto pesa la rutina
		Node* node;				//Nodo que tiene la rutina
	} Routine;

	int unserializeFSFilesInfo(char* infoString);

	void freeNode(Node* node);

	void freeBlock(Block* block);

	void freeTask(MartaTask* task);

	void freeRoutine(Routine* routine);
#endif

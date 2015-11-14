#ifndef MEMORY_H_
#define MEMORY_H_

	#include <stdbool.h>
	#include <socket.h>

	void initMemory(t_config* memoryConfig);



	typedef struct
	{
		int num;
		bool present;
		bool modified;
		bool used;
		int frame;
		int frame_timestamp;		// timestamp en que se asigno frame
		int modified_timestamp;		// timestamp de ultima modificacion
	} t_page;

	typedef struct
	{
		int id;
		t_list * pages;
		t_page * clock_pointer;
		socket_connection * connection;
		int access;
		int faults;

	} t_process;

	typedef struct
	{
		int pid;
		int page;
		char * data;
		bool arriving;
		socket_connection * connection;
	} t_write_petition;

	typedef struct
	{
		int pid;
		int page;
		bool arriving;
		socket_connection * connection;
	} t_read_petition;

	typedef struct
	{
		int pid;
		int page;
		int frame;
	} t_translation;


	// Inicializa las estructuras de memoria
	void initMemory(t_config* memoryConfig);

	//-###############################################################################################-//
	//-###-[PROCESSES]-###############################################################################-//
	//-###############################################################################################-//

	void addProcess(int id, int pages_q, socket_connection * connection);

	// Devuelve un proceso segun su ID
	t_process * getProcess(int id);

	// Determina si existe un proceso segun su ID
	bool existsProcess(int id);

	// Elimina una tabla de paginas
	void deletePages(t_list * pages);

	// Elimina un proceso
	void deleteProcess(int id);

	// Devuelve una pagina de proceso segun su numero
	t_page * getPage(int pid, int num);

	//-###############################################################################################-//
	//-###-[TLB]-#####################################################################################-//
	//-###############################################################################################-//

	// Agrega traduccion a la TLB
	void addTranslation(int pid, int page, int frame);

	// Devuelve un traduccion segun el PID y pagina
	t_translation * getTranslation(int pid, int page);

	// Elimina todos los registros de la TLB
	void clearTLB();

	// Imprime la tasa de aciertos de la TLB
	void printHitRate();

	//-###############################################################################################-//
	//-###-[MEMORY]-##################################################################################-//
	//-###############################################################################################-//

	// Corre retardo de acceso a memoria
	void sleepAccessMemory();

	// Ingresa datos en un frame
	void setMemoryData(int frame, char * data, bool sleep);

	// devuelve los datos de un frame
	char * getMemoryData(int frame, bool sleep);

	// Vacia la memoria y deja los frames sin modificar
	void clearMemory();

	// Imprime la memoria en el LOG
	void printMemory();

	//-###############################################################################################-//
	//-###-[FRAMES]-##################################################################################-//
	//-###############################################################################################-//

	// Selecciona la pagina que debe salir por FIFO
	t_page * selectFrame_FIFO(t_list * presents);

	// Selecciona la pagina que debe salir por LRU
	t_page * selectFrame_LRU(t_list * presents);

	// Selecciona la pagina que debe salir por CLOCK
	t_page * selectFrame_CLOCK_M(int pid, t_list * presents);

	// Selecciona la pagina que debe salir
	t_page * selectFrame(int pid, t_list * presents);

	// Devuelve numero de frame si no esta disponible devuelve -1
	int getNumFrame(int pid, int page_num);

	// Devuelve un marco libre
	int getFreeFrame();

	// Asigna un marco a una pagina. devuelve true si la asignacion fue inmediata
	bool assignFrame(int pid, t_page * page);

	//-###############################################################################################-//
	//-###-[PETITIONS]-###############################################################################-//
	//-###############################################################################################-//

	// Ejecuta escritura
	bool runWrite(t_write_petition * write_petition);

	// Ejecuta lectura
	bool runRead(t_read_petition * read_petition);

	// Ejecuta cola de peticiones de escritura
	void runWritePetitions();

	// Ejecuta cola de peticiones de lectura
	void runReadPetitions();

	// Ejecuta peticiones de escritura y lectura
	void runPetitions();

	// Agrega peticion de escritura
	void addWritePetition(int pid, int page, char * data, socket_connection * connection);

	// Agrega peticion de lectura
	void addReadPetition(int pid, int page, socket_connection * connection);

#endif

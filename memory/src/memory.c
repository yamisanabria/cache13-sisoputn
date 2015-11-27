#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/bitarray.h>
#include <commons/temporal.h>
#include <commons/process.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <socket.h>
#include <utils.h>

#include "shared.h"
#include "memory.h"
#include "connections.h"

int frames_max;				// Cantidad máxima de marcos por proceso
int frames_q;    			// Cantidad de frames
int frame_size; 			// Tamaño de frame
int frames_free;			// Frames libres
char * frame_algorithm; 	// Algoritmo de seleccion de frame

int TLB_q;					// Cantidad de traduciones de la TLB
float TLB_access;			// Cantidad de accesos intentados
float TLB_hits;				// Cantidad de aciertos

char ** memory;				// Memoria reservada
bool * frames;				// Estados de los Frames (Libres/Ocupados)
t_list * processes;			// Lista de procesos
t_queue * TLB;				// Lista de traducciones de la TLB

t_queue * write_petitions;	// Cola de peticiones de escritura
t_queue * read_petitions;	// Cola de peticion de lectura

int sleep_access_memory;	// Segundos de retardo al acceder a la memoria

// Inicializa las estructuras de memoria
void initMemory(t_config* memoryConfig)
{
	frames_max 				= config_get_int_value(memoryConfig, "MAXIMO_MARCOS_POR_PROCESO");
	frames_q 				= config_get_int_value(memoryConfig, "CANTIDAD_MARCOS");
	frame_size 				= config_get_int_value(memoryConfig, "TAMANIO_MARCOS");
	frame_algorithm			= config_get_string_value(memoryConfig, "ALGORITMO_REEMPLAZO");
	frames_free = frames_q;

	TLB_q					= config_get_int_value(memoryConfig, "ENTRADAS_TLB");
	sleep_access_memory		= config_get_int_value(memoryConfig, "RETARDO_MEMORIA");

	//reservo la memoria necesaria
	memory = malloc(sizeof(char*)*frames_q);


	//Inicializacion de frames vacios
	frames = malloc(sizeof(bool) * frames_q);
	int i;
	for(i = 0; i < frames_q; i++)
	{
		memory[i] = string_new();
		frames[i] = false;
	}

	//Inicializacion de la TLB
	TLB = queue_create();

	//Inicializacion de lista de procesos
	processes = list_create();

	//Inicializo colas de escritura y lectura
	write_petitions = queue_create();
	read_petitions = queue_create();
}

//-###############################################################################################-//
//-###-[PROCESSES]-###############################################################################-//
//-###############################################################################################-//

void addProcess(int id, int pages_q, socket_connection * connection)
{
	//Creo tabla de paginas
	t_list * pages = list_create();
	int i;
	t_page * page;
	for(i = 0; i < pages_q; i++)
	{
		page = malloc(sizeof(t_page));
		page->num = i;
		page->present = false;
		page->modified = false;
		page->used = false;
		page->frame = -1;
		page->frame_timestamp = 0;
		page->modified_timestamp = 0;
		list_add(pages, page);
	}

	//creo proceso
	t_process * process = malloc(sizeof(t_process));
	process->id = id;
	process->pages = pages;
	process->clock_pointer = NULL;
	process->connection = connection;
	process->access = 0;
	process->faults = 0;
	list_add(processes, process);
}

// Devuelve un proceso segun su ID
t_process * getProcess(int id)
{
	int _get(t_process * process)
	{return process->id == id;}

	return list_find(processes, (void*) _get);
}

// Determina si existe un proceso segun su ID
bool existsProcess(int id)
{
	return getProcess(id) != NULL;
}

// Elimina una tabla de paginas
void deletePages(t_list * pages)
{
	void _destroy(t_page * page)
	{free(page);}

	int i;
	t_page * page;
	for(i = 0; i < list_size(pages); i++)
	{
		page = list_get(pages, i);
		if(page->present)
		{
			frames[page->frame] = false;
			setMemoryData(page->frame, string_new(), false);
			frames_free++;
		}
	}

	list_clean_and_destroy_elements(pages, (void*)_destroy);
}

// Elimina un proceso
void deleteProcess(int id)
{
	t_process * process = getProcess(id);

	log_info(logger, "Accesos/Fallas: %d/%d.", process->access, process->faults);

	deletePages(process->pages);

	int _get(t_process * process)
	{return process->id == id;}

	void _destroy(t_process * process)
	{free(process);}

	list_remove_and_destroy_by_condition(processes, (void*)_get, (void*)_destroy);
}

// Devuelve una pagina de proceso segun su numero
t_page * getPage(int pid, int num)
{
	int _get(t_page * page)
	{return page->num == num;}

	t_process * process = getProcess(pid);
	return list_find(process->pages, (void*)_get);
}

//-###############################################################################################-//
//-###-[TLB]-#####################################################################################-//
//-###############################################################################################-//

// Agrega traduccion a la TLB
void addTranslation(int pid, int page, int frame)
{
	if(TLB_q == 0)
		return;

	t_translation * translation = malloc(sizeof(t_translation));
	translation->pid = pid;
	translation->page = page;
	translation->frame = frame;
	if(queue_size(TLB) > TLB_q)
	{
		t_translation * select = queue_pop(TLB);
		free(select);
	}

	queue_push(TLB, translation);
}

// Devuelve un traduccion segun el PID y pagina
t_translation * getTranslation(int pid, int page)
{
	int _get(t_translation * translation)
	{return translation->pid == pid && translation->page == page;}

	return list_find(TLB->elements, (void*) _get);
}

// Elimina todos los registros de la TLB
void clearTLB()
{
	void _destroyer(t_translation * translation)
	{free(translation);}
	queue_clean_and_destroy_elements(TLB, (void*)_destroyer);
}

// Imprime la tasa de aciertos de la TLB
void printHitRate()
{
	if(TLB_access > 0)
		log_info(logger, "Referencias: %f. Aciertos: %f. Tasa de aciertos: %f.", TLB_access, TLB_hits, (TLB_hits / TLB_access));
	else
		log_info(logger, "TLB sin movimientos.");
}

//-###############################################################################################-//
//-###-[MEMORY]-##################################################################################-//
//-###############################################################################################-//

// Corre retardo de acceso a memoria
void sleepAccessMemory()
{
	log_info(logger, "Retardo de %d segundos.", sleep_access_memory);
	usleep(sleep_access_memory);
}

// Ingresa datos en un frame
void setMemoryData(int frame, char * data, bool sleep)
{
	if(sleep)
		sleepAccessMemory();

	free(memory[frame]);
	memory[frame] = string_duplicate(data);
}

// devuelve los datos de un frame
char * getMemoryData(int frame, bool sleep)
{
	if(sleep)
		sleepAccessMemory();

	return memory[frame];
}

// Vacia la memoria y deja los frames sin modificar
void clearMemory()
{
	int i, j;
	for(i = 0; i < frames_q; i++)
	{
		free(memory[i]);
		memory[i] = string_duplicate("");
	}

	t_list * ps;
	for(i = 0; i < list_size(processes); i++)
	{
		ps = ((t_process*)list_get(processes, i))->pages;
		for(j = 0; j < list_size(ps); j++)
			((t_page*)list_get(ps, j))->modified = false;
	}
}

// Imprime la memoria en el LOG
void printMemory()
{
	int i;
	for(i = 0; i < frames_q; i++)
		log_info(logger, getMemoryData(i, false));
}

//-###############################################################################################-//
//-###-[FRAMES]-##################################################################################-//
//-###############################################################################################-//

// Selecciona la pagina que debe salir por FIFO
t_page * selectFrame_FIFO(t_list * presents)
{
	int i;
	int smallTime = 2000000000;
	t_page * selected;
	t_page * page;
	for(i = 0; i < list_size(presents); i++)
	{
		page = list_get(presents, i);
		if(page->frame_timestamp < smallTime)
		{
			smallTime = page->frame_timestamp;
			selected = page;
		}
	}

	return selected;
}

// Selecciona la pagina que debe salir por LRU
t_page * selectFrame_LRU(t_list * presents)
{
	int i;
	int smallTime = 2000000000;
	t_page * selected;
	t_page * page;
	for(i = 0; i < list_size(presents); i++)
	{
		page = list_get(presents, i);
		if(page->modified_timestamp < smallTime)
		{
			smallTime = page->modified_timestamp;
			selected = page;
		}
	}

	return selected;
}

// Selecciona la pagina que debe salir por CLOCK
t_page * selectFrame_CLOCK_M(int pid, t_list * presents)
{
	t_process * process = getProcess(pid);

	//Inicializo la primera vez el puntero al primer marco
	if(process->clock_pointer == NULL)
		process->clock_pointer = list_get(presents, 0);

	//Ordeno como pila
	int _comparator(t_page * a, t_page * b)
	{return a->frame_timestamp < b->frame_timestamp;}
	list_sort(presents, (void *)_comparator);

	//defino donde esta el puntero
	int nextIndex = 0;
	for(nextIndex = 0; nextIndex < list_size(presents); nextIndex++)
		if(((t_page *)list_get(presents, nextIndex))->frame == process->clock_pointer->frame)
			break;

	int method = 1;
	int count = 0;
	while(true)
	{
		count++;

		if(method == 1 && !process->clock_pointer->used && !process->clock_pointer->modified)
			return process->clock_pointer;

		if(method == 2)
		{
			if(!process->clock_pointer->used && process->clock_pointer->modified)
				return process->clock_pointer;

			process->clock_pointer->used = false;
		}

		if(count == list_size(presents))
		{
			count = 0;
			method = method == 1 ? 2 : 1;
		}

		//cambio puntero
		nextIndex = ++nextIndex >= list_size(presents) ? 0 : nextIndex;
		process->clock_pointer = list_get(presents, nextIndex);
	}
}

// Selecciona la pagina que debe salir
t_page * selectFrame(int pid, t_list * presents)
{
	if(strcmp(frame_algorithm, "FIFO") == 0)
		return selectFrame_FIFO(presents);

	if(strcmp(frame_algorithm, "LRU") == 0)
		return selectFrame_LRU(presents);

	if(strcmp(frame_algorithm, "CLOCK-M") == 0)
		return selectFrame_CLOCK_M(pid, presents);

	return NULL;
}

// Devuelve numero de frame si no esta disponible devuelve -1
int getNumFrame(int pid, int page_num)
{
	t_translation * translation = getTranslation(pid, page_num);
	TLB_access += 1.0f;
	if(translation != NULL)
	{
		TLB_hits += 1.0f;
		return translation->frame;
	}

	sleepAccessMemory();

	t_page * page = getPage(pid, page_num);
	if(page != NULL)
		if(page->present)
			return page->frame;

	return -1;
}

// Devuelve un marco libre
int getFreeFrame()
{
	int i;
	for(i = 0; i < frames_q; i++)
	{
		if(!frames[i])
		{
			frames[i] = true;
			frames_free--;
			return i;
		}
	}
	return -1;
}

// Asigna un marco a una pagina. devuelve true si la asignacion fue inmediata
bool assignFrame(int pid, t_page * page)
{
	int _filter(t_page * page)
	{return page->present;}

	page->frame_timestamp = getTimestamp();
	page->modified_timestamp = getTimestamp();
	page->modified = false;
	page->used = true;

	t_process * process = getProcess(pid);

	t_list * presents = list_filter(process->pages, (void*)_filter);

	// Asigno frame libre (en caso que haya) si el proceso no supero el maximo permitido
	if(list_size(presents) < frames_max && frames_free > 0)
	{
		page->frame = getFreeFrame();
		/*page->present = false;
		addTranslation(pid, page->num, page->frame);
		sw_getPage(pid, page->num);
		return false;*/
		page->present = true;
                addTranslation(pid, page->num, page->frame);
                return true;
	}
	// Selecciono un frame (si es que hay)
	else if(list_size(presents) > 0)
	{
		// Selecciono frame
		t_page * select = selectFrame(pid, presents);
		page->frame = select->frame;
		addTranslation(pid, page->num, page->frame);

		select->present = false;
		select->frame = -1;

		//Si esta modificado hago swapping caso contrario solo asigno la pagina
		if(select->modified)
		{
			char * data = getMemoryData(page->frame, false);
			sw_swapping(pid, select->num, data, page->num);
			return false;
		}
		else
		{
			/*page->present = false;
			sw_getPage(pid, page->num);
			return false;*/
			page->present = true;
                	return true;
		}
	}
	// Sin frames asignados y sin frames disponibles: aborto
	else
	{
		deleteProcess(pid);
		sw_endProcess(pid);
		cpu_noSpace(process->connection->socket, pid);
		return false;
	}
}

//-###############################################################################################-//
//-###-[PETITIONS]-###############################################################################-//
//-###############################################################################################-//

// Ejecuta escritura
bool runWrite(t_write_petition * write_petition)
{
	if(!write_petition->arriving)
		getNumFrame(write_petition->pid, write_petition->page);

	t_page * page = getPage(write_petition->pid, write_petition->page);
	t_process * process = getProcess(write_petition->pid);
	if(page->present)
	{
		process->access++;

		//seteo datos de pagina y aviso que se realizo ok
		setMemoryData(page->frame, write_petition->data, true);
		page->modified = true;
		page->used = true;
		page->modified_timestamp = getTimestamp();
		cpu_writeOk(write_petition->connection->socket);
		return true;
	}
	else if(!write_petition->arriving)
	{
		process->faults++;

		write_petition->arriving = true;
		if(assignFrame(write_petition->pid, page))
			return runWrite(write_petition);
	}

	return false;
}

// Ejecuta lectura
bool runRead(t_read_petition * read_petition)
{
	if(!read_petition->arriving)
		getNumFrame(read_petition->pid, read_petition->page);

	t_page * page = getPage(read_petition->pid, read_petition->page);
	t_process * process = getProcess(read_petition->pid);
	if(page->present)
	{
		process->access++;

		//obtengo datos de pagina y los envio
		char * data = getMemoryData(page->frame, true);
		page->used = true;
		cpu_frameData(read_petition->connection->socket, page->num, data);
		return true;
	}
	else if(!read_petition->arriving)
	{
		process->faults++;

		read_petition->arriving = true;
		if(assignFrame(read_petition->pid, page))
			return runRead(read_petition);
	}

	return false;
}

// Ejecuta cola de peticiones de escritura
void runWritePetitions()
{
	int i;
	int q = queue_size(write_petitions);
	t_write_petition * write_petition;
	for(i = 0; i < q; i++)
	{
		write_petition = queue_pop(write_petitions);
		if(runWrite(write_petition))
			free(write_petition);
		else
			queue_push(write_petitions, write_petition);
	}
}

// Ejecuta cola de peticiones de lectura
void runReadPetitions()
{
	int i;
	int q = queue_size(read_petitions);
	t_read_petition * read_petition;
	for(i = 0; i < q; i++)
	{
		read_petition = queue_pop(read_petitions);
		if(runRead(read_petition))
			free(read_petition);
		else
			queue_push(read_petitions, read_petition);
	}
}

// Ejecuta peticiones de escritura y lectura
void runPetitions()
{
	runWritePetitions();
	runReadPetitions();
}

// Agrega peticion de escritura
void addWritePetition(int pid, int page, char * data, socket_connection * connection)
{
	t_write_petition * write = malloc(sizeof(t_write_petition));
	write->pid = pid;
	write->page = page;
	write->connection = connection;
	write->data = data;
	write->arriving = false;
	queue_push(write_petitions, write);

	runWritePetitions();
}

// Agrega peticion de lectura
void addReadPetition(int pid, int page, socket_connection * connection)
{
	t_read_petition * read = malloc(sizeof(t_read_petition));
	read->pid = pid;
	read->page = page;
	read->connection = connection;
	read->arriving = false;
	queue_push(read_petitions, read);

	runReadPetitions();
}



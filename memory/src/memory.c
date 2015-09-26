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

int frames_max;			// Cantidad máxima de marcos por proceso
int frames_q;    		// Cantidad de frames
int frame_size; 		// Tamaño de frame

char * memory;			// Memoria reservada
bool * frames;			// Estados de los Frames (Libres/Ocupados)
t_list * processes;		// Lista de procesos

t_queue * write_petitions;	// Cola de peticiones de escritura
t_queue * read_petitions;	// Cola de peticion de lectura

// Inicializa las estructuras de memoria
void initMemory(t_config* memoryConfig)
{
	frames_max 		= config_get_int_value(memoryConfig, "MAXIMO_MARCOS_POR_PROCESO");
	frames_q 		= config_get_int_value(memoryConfig, "CANTIDAD_MARCOS");
	frame_size 		= config_get_int_value(memoryConfig, "TAMANIO_MARCOS");

	//reservo la memoria necesaria
	memory = malloc(frames_q * frame_size + 1);
	frames = list_create();

	//Inicializacion de frames vacios
	frames = malloc(sizeof(bool) * frames_q);
	int i;
	for(i = 0; i < frames_q; i++)
		frames[i] = false;
}

//-###############################################################################################-//
//-###-[PROCESSES]-###############################################################################-//
//-###############################################################################################-//

void startProcess(int id, int pages_q)
{
	//Creo tabla de paginas
	t_list * pages = list_create();
	int i;
	t_page * page;
	for(i = 0; i < pages_q; i++)
	{
		page->num = i;
		page->present = false;
		page->modified = false;
		page->frame = -1;
		list_add(pages, page);
	}

	//creo proceso
	t_process * process = malloc(sizeof(t_process));
	process->id = id;
	process->pages = pages;
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
			frames[page->frame] = false;
	}

	list_clean_and_destroy_elements(pages, (void*)_destroy);
}

// Elimina un proceso
void deleteProcess(int id)
{
	t_process * process = getProcess(id);
	deletePages(process->pages);

	int _get(t_process * process)
	{return process->id == id;}

	void _destroy(t_process * process)
	{free(process);}

	list_remove_and_destroy_by_condition(processes, (void*)_get, (void*)_destroy);
}

//-###############################################################################################-//
//-###-[PAGES]-###################################################################################-//
//-###############################################################################################-//

void setMemoryData(int frame, char * data)
{
	//todo: hacer
}

void getMemoryData(int frame)
{
	//todo: hacer
}

// Devuelve una pagina de proceso segun su numero
t_page * getPage(int pid, int num)
{
	int _get(t_page * page)
	{return page->num == num;}

	t_process * process = getProcess(pid);
	return list_find(process->pages, (void*)_get);
}

// Ejecuta escritura
bool write(t_write_petition * write_petition)
{
	t_page * page = getPage(write_petition->pid, write_petition->page_num);
	if(page->present)
	{
		setMemoryData(page->frame, write_petition->data);
		//todo: decirle al CPU que se escribio bien
		return true;
	}

	return false;
}

// Ejecuta lectura
bool read(t_read_petition * read_petition)
{
	t_page * page = getPage(read_petition->pid, read_petition->page_num);
	if(page->present)
	{
		getMemoryData(page->frame);
		//todo: enviarle la data al CPU
		return true;
	}

	return false;
}

// Ejecuta cola de peticiones de escritura
void runWritePetitions()
{
	int i;
	t_write_petition * write_petition;
	for(i = 0; i < queue_size(write_petitions); i++)
	{
		write_petition = queue_pop(write_petitions);
		if(write(write_petition->pid, write_petition->page_num, write_petition->data))
			free(write_petition);
		else
			queue_push(write_petitions, write_petition);
	}
}

// Ejecuta cola de peticiones de lectura
void runReadPetitions()
{
	int i;
	t_read_petition * read_petition;
	for(i = 0; i < queue_size(write_petitions); i++)
	{
		read_petition = queue_pop(read_petitions);
		if(read(read_petition->pid, read_petition->page_num))
			free(read_petition);
		else
			queue_push(write_petitions, read_petition);
	}
}

// Ejecuta peticiones de escritura y lectura
void runPetitions()
{
	runWritePetitions();
	runReadPetitions();
}



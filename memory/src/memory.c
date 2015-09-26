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

int frames_max;		// Cantidad máxima de marcos por proceso
int frames_q;    	// Cantidad de frames
int frame_size; 	// Tamaño de frame

char * memory;		// Memoria reservada
t_list * frames;	// Frames de la memoria

void initMemory(t_config* memoryConfig)
{
	frames_max 		= config_get_int_value(memoryConfig, "MAXIMO_MARCOS_POR_PROCESO");
	frames_q 		= config_get_int_value(memoryConfig, "CANTIDAD_MARCOS");
	frame_size 		= config_get_int_value(memoryConfig, "TAMANIO_MARCOS");

	//reservo la memoria necesaria
	memory = malloc(frames_q * frame_size + 1);
	frames = list_create();

	//Inicializacion de frames
	int i;
	t_frame * frame;
	for(i = 0; i < frames_q; i++)
	{
		frame = malloc(sizeof(t_frame));
		frame->pid = -1;
		frame->num = 0;
		list_add(frames, frame);
	}
}



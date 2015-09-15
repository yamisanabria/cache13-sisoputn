#ifndef SHARED_H_
#define SHARED_H_

#include <stdbool.h>
#include <commons/log.h>
#include <commons/collections/list.h>


int				port;						/* Puerto del FS */
int				minNodos;					/* Cantidad minima de nodos para que el FS pase a Operativo */
char* 			martaIP; 					/* Dirección IP de la computadora que está ejecutando el proceso MaRTA */
int 			martaPort; 					/* Puerto donde el proceso MaRTA está  escuchando nuevas conexiones. */
bool			operative;			/* Determina si el FS esta operativo */
t_log*			logg;						/* Variable para logs de FS */

// Listas
	t_list * directories;
	t_list * files;
	t_list * blocks;
	t_list * copys;
	t_list * nodos;

#endif

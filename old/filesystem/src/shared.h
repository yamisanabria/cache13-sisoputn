#ifndef SHARED_H_
#define SHARED_H_

	#include <stdbool.h>
	#include <commons/log.h>
	#include <commons/collections/list.h>
#include <socket.h>




	int				port;						/* Puerto del FS */
	int				minNodos;					/* Cantidad minima de nodos para que el FS pase a Operativo */
	char* 			martaIP; 					/* Dirección IP de la computadora que está ejecutando el proceso MaRTA */
	int 			martaPort; 					/* Puerto donde el proceso MaRTA está  escuchando nuevas conexiones. */
	bool			operative;					/* Determina si el FS esta operativo */
	t_log*			logg;						/* Variable para logs de FS */
	int				count_copies;				/* Cantidad de copias a realizar por cada carga de archivo */
	socket_connection * martaConnection;			/* Estructura de conexión perteneciente a marta	*/

	// Listas
	t_list * directories_list;
	t_list * files_list;
	t_list * blocks_list;
	t_list * copys_list;
	t_list * nodos_list;

	typedef enum {OFF, DOWNLOAD_FILE, DOWNLOAD_BLOCK, MOVE} getBlockStatus_t;
	getBlockStatus_t getBlockStatus;

#endif

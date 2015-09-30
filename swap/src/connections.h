/*
 * connections.h
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */
#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_


//DEFINICIONES DE LAS FUNCIONES DE LA CONEXIÓN

/* Función que obtiene el puerto desde el archivo de configuración */
void setSwapPort(t_config *config);

/* Función que inicializa el diccionario de funciones a ser llamadas por algún proceso "cliente" */
void initDictionary();

/* Función que me avisa que se ha conectado un proceso Administrador de Memoria */
void memAdmConnected(socket_connection* conn);

/* Función que me avisa que se ha desconectado un proceso Administrador de Memoria */
void memAdmDisconnected(socket_connection * conn);

/* Función que crea un socket_server para estar a la escucha de algun proceso Administrador de Memoria*/
void startListener();


//DEFINICIONES DE LAS FUNCIONES DE COMUNICACIÓN

/* El administrador de memoria nos indica que comenzó un proceso */
void mem_sw_startProcess(socket_connection *conn, char **args);

//El administrador de memoria nos solicita una página (lectura)
void mem_sw_getPage(socket_connection *conn, char **args);

//El administrador de memoria nos pide escribir una página
void mem_sw_setPage(socket_connection *conn, char **args);

//El administrador de memoria indica que finalizó un proceso
void mem_sw_endProcess(socket_connection *conn, char **args);


#endif	/* CONNECTIONS_H_ */

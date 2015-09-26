/*
 * swap.h
 *
 *  Created on: 9/9/2015
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

// STRUCTS

/* Estructura para adminsitrar el espacio utilizado */
typedef struct list_proc
	 {
	     int pid ;
		 int offset;
		 int pageQty;
   	} list_proc;

/* Estructura para adminsitrar el espacio libre */
//typedef struct list_holes
//	 {
//	     int offset;
//		 int pageQty;
//	} list_holes;

//DEFINICIONES DE LAS FUNCIONES PROPIAS

/* Función que lee el archivo de configuración */
void readFileConfig();

/* Función que inicializa el log */
void initLog();

/* Función que creará el swap.data con su configuración correspondiente */
void initSwap();

/* Función inicializa la lita para administrar el espacio utilizado y el libre */
void initList();

/* Función que asignará tamaño necesario para que el proceso cuya id es "pid" sea guardado */
void startProcess(int pid, int pageQty);


#endif /* SWAP_H_ */

/*
 * tests.c
 *
 *  Created on: 6/10/2015
 *      Author: utnso
 */
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
#include <socket.h>
#include "connections.h"
#include "shared.h"
#include "swap.h"
#include "utils.h"

socket_connection *c;	/* Estructura dummy para poder utilizar las funciones que la requieran */


void startTesting(){

	//Iniciar mProcs
	startProcess(c,1,2); 			     /* Paso 1 */
	startProcess(c,2,3);			     /* Paso 2 */
	startProcess(c,3,3);			     /* Paso 3 */
	startProcess(c,4,2); 			     /* Paso 4 */

	//Escribir páginas de mProcs
	char *p1 = string_duplicate("the");  /* Paso 5 */
	pageWriteRequest(c,1,0,p1);          /* Paso 5 */
	char *p2 = string_duplicate("cat");  /* Paso 6 */
	pageWriteRequest(c,1,1,p2);			 /* Paso 6 */
	char *p3 = string_duplicate("hola"); /* Paso 7 */
	pageWriteRequest(c,2,0,p3);			 /* Paso 7 */
	char *p4 = string_duplicate("mund"); /* Paso 8 */
	pageWriteRequest(c,2,1,p4);		     /* Paso 8 */
	char *p5 = string_duplicate("o");	 /* Paso 9 */
	pageWriteRequest(c,2,2,p5);			 /* Paso 9 */
	char *p6 = string_duplicate("Mati"); /* Paso 10 */
	pageWriteRequest(c,3,0,p6);			 /* Paso 10 */
	char *p7 = string_duplicate("alls"); /* Paso 11 */
	pageWriteRequest(c,3,1,p7);			 /* Paso 11*/
	char *p8 = string_duplicate("tar");  /* Paso 12 */
	pageWriteRequest(c,3,2,p8);			 /* Paso 12 */

	//Leer páginas de mProcs
	pageReadRequest(c,1,0);				 /* Paso 13 */
	pageReadRequest(c,1,1);				 /* Paso 14 */
	pageReadRequest(c,2,0);				 /* Paso 15 */
	pageReadRequest(c,2,1);				 /* Paso 16 */
	pageReadRequest(c,2,2);				 /* Paso 17 */
	pageReadRequest(c,3,0);				 /* Paso 18 */
	pageReadRequest(c,3,1);				 /* Paso 19 */
	pageReadRequest(c,3,2);				 /* Paso 20 */

	//Finalizar mProc
	endProcess(c,2);					 /* Paso 21 */

	//Iniciar mProc (requirá compactación)
	startProcess(c,5,4);				 /* Paso 22 */

	//Escribir páginas de mProcs
	char *p9 = string_duplicate("NEO");  /* Paso 23 */
	pageWriteRequest(c,5,3,p9);          /* Paso 23 */

	//Finalizar mProc
	endProcess(c,3);					 /* Paso 24 */

	//Iniciar mProc (requirá compactación)
	startProcess(c,6,1);				 /* Paso 25 */

	//Escribir páginas de mProcs
	char *p10 = string_duplicate("fork"); /* Paso 26 */
	pageWriteRequest(c,6,0,p10);          /* Paso 26 */

	//Swapping /* Paso 27 */
	char *p11 = string_duplicate("bash");
	pageWriteRequest(c,5,0,p11);
	pageReadRequest(c,5,3);


}



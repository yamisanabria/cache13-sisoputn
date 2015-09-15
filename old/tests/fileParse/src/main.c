#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

char* concat(char *s1, char *s2) {
	    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 para el /0
	    //Habría que chequear si hay errores en el malloc
	    strcpy(result, s1);
	    strcat(result, s2);
	    return result;
	}

// Ya existe en el UTIL
void createFile(char * path, char * data)
{
	//abro archivo
	FILE *fp;
	fp = fopen(path, "rb+");

	//Si no existe archivo lo creo
	if(fp == NULL)
		fp = fopen(path, "wb");

	//Escribo
	fprintf(fp, "%s", data);

	//Cierro
	fclose(fp);
}

// Copiarla en el Util
// Retorna el tamaño de un fileDescriptor
int fileSize(int file_descriptor)
{
	struct stat buffer;
	fstat(file_descriptor, &buffer);
	return buffer.st_size;
}

void splitFileUpIntoBlocks(char* file)
{
	int fileDescriptor;					// Descriptor del archivo
	double f_size;						// Tamaño del archivo
	int cantBlock;						// Cantidad de Bloques (int)
	int blockLength = 20*1024*1024;		// Tamaño del bloque: 20MB
	char *lines;						// Contenido del bloque en memoria mapeada
	char * block;						// Bloque generado
	char* exceeded; 					// Exceso para el proximo bloque
	char * tmp;
	int i,j;
	char * lines_cpy;
	int realStrLen;

	FILE * fd = fopen("/home/utnso/mmap","wb");

	// @ToDO: Esto se comprobaría antes de llamar a esta funcion (no es necesario)
	if((fileDescriptor = open(file, O_RDWR)) == -1)
	{
		printf("Error al abrir el archivo\n");
		exit(1);
	}

	// Obtengo el tamaño del archivo
	f_size = fileSize(fileDescriptor);
	cantBlock = (int) ceil(f_size / blockLength);

	exceeded = string_new();

	// Recorro bloques de 20MB exactos (Realizo recorrido exta si en el ultimo bloque me quedo excedente)
	// Por cada recorrido se crea un bloque
	for(i = 0; i < cantBlock || !string_is_empty(exceeded); i++)
	{
		printf("%d\n", i);
		block = string_new();

		// Guardo excedente al comienzo del bloque
		if(!string_is_empty(exceeded))
			string_append(&block, exceeded);
		free(exceeded);
		exceeded = string_new();

		// No trato de leer un bloque del archivo si estoy en el recorrido extra por excedente
		if(i < cantBlock)
		{
			// Leo 20MB del archivo y los guardo en el bloque
			lines = mmap((caddr_t) 0, blockLength, PROT_READ, MAP_SHARED, fileDescriptor,blockLength*i);

			realStrLen = i < cantBlock - 1 ? blockLength : (int)f_size % blockLength;
			lines_cpy = malloc(realStrLen+1);
			memcpy(lines_cpy, lines, realStrLen);
			string_append(&block, lines_cpy);
			free(lines_cpy);

			//string_append(&block, lines); //@ToDo: en valgrind da error el string_append al querer hacer length de lines...
			munmap(lines, blockLength);
		}


		// No recorro hacia atras si el ultimo caracter es el flag de final (seguro que nunca pasa)
		// Excepto que el bloque sea > que 20MB
		if(block[strlen(block)-1] != '\0' || strlen(block) > blockLength)
		{

			// Recorro hacia atras hasta encontrar salto de linea, si encontramos una: Se corta y guarda el excedente
			do
			{

				for(j = strlen(block) - 1; j >= 0; j--)
				{
					if
					(
						(block[j] == '\n' && (j != strlen(block) - 1 || strlen(block) <= blockLength)) || //caracter es enter y no es el ultimo caracter(salvo que el bloque desborde los 20MB
						(i >= cantBlock - 1 && strlen(block) <= blockLength) //Posible ultimo bloque y contenido del bloque menor a 20MB
					)
					{
						tmp = string_new();
						string_append(&tmp, &block[j+1]);
						string_append(&tmp, exceeded);
						free(exceeded);
						exceeded = tmp;
						block[j+1] = '\0';
						break;
					}
				}
			} while(strlen(block) > blockLength); // Repito el proceso hasta que el bloque sea <= 20MB
		}

		fprintf(fd, "%s", block);

		free(block);
	}

	free(exceeded);

	/* Cierro el file descriptor */
	fclose(fd);
}



void test()
{
	int fileDescriptor;					// Descriptor del archivo
	char *lines;						// Contenido del bloque en memoria mapeada
	char * block;						// Bloque generado
	int blockLength = 1*1024*1024;
	char * lines_char = malloc(blockLength + 1);


	fileDescriptor = open("/home/utnso/hourly.txt", O_RDONLY);

	block = string_new();

	// Leo 20MB del archivo y los guardo en el bloque
	lines = (char*)mmap((caddr_t) 0, blockLength, PROT_READ,  MAP_PRIVATE | MAP_NORESERVE, fileDescriptor, 0);


	munmap(lines, blockLength);

	free(block);
	close(fileDescriptor);
}

int main(){

	//test();
	splitFileUpIntoBlocks("/home/utnso/hourly.txt");
	return 0;
}



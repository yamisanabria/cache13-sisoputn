/*
 *
 * Tomado de http://rosettacode.org/wiki/Read_a_specific_line_from_a_file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_nth_line(char *codfile, int line_no )
{
    FILE* fp = fopen(codfile, "r");
    if (fp == NULL) {
      printf("no existe archivo %s\n", codfile);
      exit(EXIT_FAILURE);
    }
    
    char * line = NULL;
    size_t len = 0;
    int i = 0;
    
    while ((getline(&line, &len, fp)) != -1) {
        if(i == line_no) {
            strtok(line, ";\n");
            return line;
        }
        i++;
    }
    fclose(fp);

    return line;
   

}

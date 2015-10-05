/*
 *
 * Tomado de http://rosettacode.org/wiki/Read_a_specific_line_from_a_file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_nth_line( FILE *fp, int line_no )
{
    char * line = NULL;
    size_t len = 0;
    int i = 0;
    
    // vuelvo a la posicion inicial
    rewind(fp);

    while ((getline(&line, &len, fp)) != -1) {
        if(i == line_no) {
            strtok(line, "\n");
            return line;
        }
        i++;
    }

    return line;
   

}

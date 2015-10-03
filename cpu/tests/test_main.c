/**
si no esta instalado cspec puede fallar;
https://github.com/pepita-remembrance/cspec
*/
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

#include "filereader.h"

context (example) {

    describe("Test file reader") {

        it("true should be equal true") {
            FILE* codfile = fopen("resource/test.cod", "r");
            if (codfile == NULL) {
              printf("no existe archivo\n");
              exit(EXIT_FAILURE);
            }
            should_string(get_nth_line(codfile, 0)) be equal to("Linea 1");
            should_string(get_nth_line(codfile, 4)) be equal to("Linea 5");
            should_string(get_nth_line(codfile, 2)) be equal to("Linea 3");
        } end
/*
        it("true shouldn't be equal to false") {
            should_bool(true) not be equal to(false);
        } end

        it("this test will fail because 10 is not equal to 11") {
            should_int(10) be equal to(11);
        } end

        skip("this test will fail because \"Hello\" is not \"Bye\"") {
            should_string("Hello") be equal to("Bye");
        } end
*/
    } end

}
//gcc cspecExample.c -o cspecExample -lcspecs
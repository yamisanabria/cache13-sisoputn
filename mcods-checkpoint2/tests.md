Pruebas
=======
[fuente](http://www.utn.so/wp-content/uploads/2015/09/2015-2C-Pruebas-Cache13-2.pdf)



## Prueba 1 - Condición mínima

Esta prueba comprueba el estado determinado como mínimo para que un trabajo práctico
sea evaluado. ​Si esto no ocurre, de todas formas es importante que el grupo asista para
evacuar sus dudas. El checkpoint es una instancia de aprendizaje y feedback.
Se deberá correr largo.cod correctamente. Al revisar los logs, se mostrarán las sentencias
ejecutadas.


## Prueba 2 - Ejecución FIFO

Se deberá correr de forma concurrente el código de largo.cod, corto.cod y largo.cod
(nuevamente). Al revisar los logs, se mostrarán las sentencias ejecutadas, en orden FIFO.


## Prueba 3 - Rechazo de proceso

Se deberá correr de forma concurrente el código de largo.cod y no_entra.cod. no_entra.cod
deberá ser rechazado.


## Prueba 4 - Caso borde

Se deberá correr el código de entra.cod. La partición de swap de 64 posiciones debería
estar completamente ocupada.


##Prueba 5 - Opcional

En el código que crea la partición de swap, se cambiará el caracter ‘/0’ con el que se
inicializa, por una A. Correr las pruebas 1 a 4, chequeando que todas las lecturas devuelvan
“AAAA”.
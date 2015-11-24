# tp-2015-2c-Aprobados

Para correr en el laboratorio;

Pasos iniciales:
```
git clone https://github.com/sisoputnfrba/tp-2015-2c-Aprobados.git
cd tp-2015-2c-Aprobados
export LD_LIBRARY_PATH=$(pwd)/includes:$LD_LIBRARY_PATH
sudo make all
```


Levantar ventanas automáticamente:
```sh
#Levanta el multiplexor de terminales, si, asi de groso.
tmux          

#Carga los modulos de cache13 en diferentes ventanas de tmux, con ctrl + b podés usar las flechas y cambiar de ventanas, más info en (google!); http://hipertextual.com/archivo/2014/09/tmux/
./tmuxrun.sh  
```


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/tp-2015-1c-power-rangers/Includes/Debug

cd Includes
mkdir Debug
cd Debug

gcc -lcommons -lpthread -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"socket.d" -MT"socket.d" -o "socket.o" "../socket.c"
gcc -lcommons -lpthread -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"utils.d" -MT"utils.d" -o "utils.o" "../utils.c"
gcc -lcommons -lpthread -shared -o "libIncludes.so"  ./socket.o ./utils.o   -lcommons -lpthread

cd ..
cd ..

cd filesystem
mkdir Debug
cd Debug
mkdir src

gcc -lcommons -lm -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/commands.d" -MT"src/commands.d" -o "src/commands.o" "../src/commands.c"
gcc -lcommons -lm -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/fileSystem.d" -MT"src/fileSystem.d" -o "src/fileSystem.o" "../src/fileSystem.c"
gcc -lcommons -lm -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/persistence.d" -MT"src/persistence.d" -o "src/persistence.o" "../src/persistence.c"
gcc  -pthread -lcommons -lm -L"/home/utnso/tp-2015-1c-power-rangers/Includes/Debug" -o "filesystem"  ./src/commands.o ./src/fileSystem.o ./src/persistence.o   -lIncludes -lm -lcommons

cd ..
cd ..


cd marta
mkdir Debug
cd Debug
mkdir src

gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"connections.d" -MT"connections.d" -o "connections.o" "../connections.c"
gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"marta.d" -MT"marta.d" -o "marta.o" "../marta.c"
gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"planning.d" -MT"planning.d" -o "planning.o" "../planning.c"
gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"scheduler.d" -MT"scheduler.d" -o "scheduler.o" "../scheduler.c"
gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"shared.d" -MT"shared.d" -o "shared.o" "../shared.c"
gcc -lcommons -lpthread -L"/home/utnso/tp-2015-1c-power-rangers/Includes/Debug" -o "marta"  ./connections.o ./marta.o ./planning.o ./scheduler.o ./shared.o   -lpthread -lIncludes -lcommons

cd ..
cd ..

cd job
mkdir Debug
cd Debug
mkdir src

gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"job.d" -MT"job.d" -o "job.o" "../job.c"
gcc -lcommons -lpthread -L"/home/utnso/tp-2015-1c-power-rangers/Includes/Debug" -L/home/utnso/workspace/tp-2015-1c-power-rangers/includes -o "job"  ./job.o   -lIncludes -lcommons -lpthread

cd ..
cd ..

cd nodo
mkdir Debug
cd Debug
mkdir src

gcc -lcommons -lpthread -I"/home/utnso/tp-2015-1c-power-rangers/Includes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/nodo.d" -MT"src/nodo.d" -o "src/nodo.o" "../src/nodo.c"
gcc -lcommons -lpthread -lm -L"/home/utnso/tp-2015-1c-power-rangers/Includes/Debug" -o "nodo"  ./src/nodo.o   -lIncludes -lm -lcommons -lpthread

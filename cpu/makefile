CC=gcc
CFLAGS= -Wall -O0 -g3
LIBFLAGS=-lcommons -pthread -L"../includes" -I"../includes" -lincludes -I"src"

all:
	$(MAKE) clean
	mkdir build
	$(MAKE) cpu

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(LIBFLAGS)

cpu: build/connections.o build/cpu.o
	$(CC) $(CFLAGS) build/connections.o build/cpu.o $(LIBFLAGS) -o cpu

# Add debug parameters and compile
debug: CC += -DDEBUG -g
debug: all

clean:
	rm -rf build
	
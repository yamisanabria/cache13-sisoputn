CC=gcc
CFLAGS= -Wall -O0 -g3
LIBFLAGS=-lcommons -pthread -L"../includes" -I"../includes" -lincludes -I"src"
OBJS=build/connections.o build/swap.o
MAIN=swap

all:
	$(MAKE) clean
	mkdir build
	$(MAKE) main

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(LIBFLAGS)

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFLAGS) -o $(MAIN)

# Add debug parameters and compile
debug: CC += -DDEBUG -g
debug: all

clean:
	rm -rf build
	rm -f $(MAIN)

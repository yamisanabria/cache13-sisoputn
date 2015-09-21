CC=gcc
CFLAGS= -Wall -O0 -g3
LIBFLAGS=-lcommons -pthread -L"../includes" -I"../includes" -lincludes -I"src"
OBJS=build/connections.o build/cpu.o

all:
	$(MAKE) clean
	mkdir build
	$(MAKE) cpu

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(LIBFLAGS)

cpu: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFLAGS) -o cpu

# Add debug parameters and compile
debug: CC += -DDEBUG -g
debug: all

clean:
	rm -rf build
	rm -f cpu
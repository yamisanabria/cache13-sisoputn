CC=gcc
CFLAGS= -Wall -O0 -g3
LIBFLAGS=-lcommons -pthread 
C_SRCS=./utils.c ./socket.c
OBJS=./utils.o ./socket.o

all:
	$(MAKE) clean
	$(MAKE) includes.so

%.o: %.c
	$(CC) $(CFLAGS) -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" $(LIBFLAGS)

includes.so: $(OBJS)
	$(CC) -shared -o "libincludes.so" $(OBJS) $(LIBFLAGS)

clean:
	rm -f socket.d socket.o utils.d utils.o includes.so

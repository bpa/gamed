.SUFFIXES:
.SUFFIXES: .o .c
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

CFLAGS = -Wall -pedantic -ansi -O2 -fPIC
INC = -I../include
LIBS = -L/usr/local/lib -levent -ldl
CC = gcc -g

all: server

server: $(OBJS)
	$(CC) -o server $(OBJS) $(LIBS)

test: gamed.so test-run

test-run:
	@make -C t

gamed.so: $(OBJS)
	gcc -fPIC -shared -s -o gamed.so $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

clean:
	rm -f *.o *.so server
	@make -C t clean


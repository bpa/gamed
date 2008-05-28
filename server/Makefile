.SUFFIXES:
.SUFFIXES: .o .c
#SOURCE != ls [^m]*.cc
#OBJS = $(SOURCE:.cc=.o)
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -Wall -pedantic -ansi -O2 -I/usr/local/include -I../include -I../games
SOFLAGS = -fpic -shared -Wl,--export-dynamic
LIBS = -L/usr/local/lib -levent
#GAMES = -L../lib -lgame
CC = gcc -g

server: $(OBJS)
	$(CC) $(CFLAGS) -o server $(OBJS) $(LIBS) $(GAMES)
	$(CC) $(CFLAGS) -Wl,-x -shared -o ../lib/libgamed.so -Wl,-soname,libgamed.so $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o server

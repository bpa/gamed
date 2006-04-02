.SUFFIXES:
.SUFFIXES: .so .cc .o
SOFLAGS = -shared -Wl,-x
CFLAGS = -Wall -g
LIBS = -L/usr/local/lib -levent
OBJS = borders.o game.o
INC = -I.. -I../../include -I/usr/local/include
CC = gcc

run: tests libgame.so

tests:
	@make -C test

libgame.so: $(OBJS)
	$(CC) $(CFLAGS) $(SOFLAGS) $(INC) $(OBJS) -o libgame.so -Wl,-soname,libgame.so
	cp libgame.so ../../lib

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o *.so
	@make -C test clean

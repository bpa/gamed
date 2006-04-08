.SUFFIXES:
.SUFFIXES: .so .cc .o
SOFLAGS = -shared -Wl,-x
CFLAGS = -Wall -pedantic -g
OBJS = game.o
INC = -I.. -I../../include -I/usr/local/include
CC = gcc

run: $(OBJS) tests

tests:
	@make -C test

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o
	@make -C test clean

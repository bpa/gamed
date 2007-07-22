.SUFFIXES:
.SUFFIXES: .o .c .h
CFLAGS = -Wall -pedantic
INC = -I.. -I../../include
CC = gcc

all: game_HiLo.so tests

game_HiLo.so: HiLo.o
	$(CC) $(CFLAGS) -Wl,-x -shared -fPIC -o game_HiLo.so HiLo.o

libHiLo.so: HiLo.o
	$(CC) $(CFLAGS) -Wl,-x -shared -fPIC -o libHiLo.so HiLo.o

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

tests: $(TESTS)
	@make -C t

clean:
	rm -f *.o *.so
	@make -C t clean

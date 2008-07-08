.SUFFIXES:
.SUFFIXES: .o .c .h
CFLAGS = -fPIC -Wall -pedantic -g
INC = -I.. -I../../include
CC = gcc

all: HiLo.o tests HiLo.so

HiLo.so: HiLo.o
	ld -fPIC -shared -s -o HiLo.so HiLo.o
	cp HiLo.so ../../server/

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

tests: $(TESTS)
	@make -C t
	t/runtests

clean:
	rm -f *.o *.so
	@make -C t clean

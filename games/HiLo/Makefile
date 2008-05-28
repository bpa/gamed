.SUFFIXES:
.SUFFIXES: .o .c .h
CFLAGS = -fPIC -Wall -pedantic -g
INC = -I.. -I../../include
CC = gcc

all: libHiLo.so tests

libHiLo.so: HiLo.o
	ld -fPIC -shared -s -o libHiLo.so HiLo.o

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

tests: $(TESTS)
	@make -C t
	t/runtests

clean:
	rm -f *.o *.so
	@make -C t clean

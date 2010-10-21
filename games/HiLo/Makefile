.SUFFIXES:
.SUFFIXES: .o .c
CFLAGS = -fPIC -Wall -pedantic
INC = -I.. -I../../include
CC = gcc

all: HiLo.so test

HiLo.so: HiLo.o
	ld -fPIC -shared -s -o HiLo.so HiLo.o

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $<

test: 
	@make -C t

clean:
	rm -f *.o *.so
	@make -C t clean

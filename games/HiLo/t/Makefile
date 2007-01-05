.SUFFIXES:
.SUFFIXES: .o .c .h

CFLAGS = -Wall -pedantic -g
OBJS := $(patsubst %.h,%.o,$(wildcard *.h))
INC = -I../.. -I../../../include
CC = gcc

build: $(OBJS)
	
.h.o:
	$(CC) $< -o $@ $(INC)

clean:
	rm -f *.o

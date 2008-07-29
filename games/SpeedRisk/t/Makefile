OBJS := $(patsubst %.c,%.o,$(wildcard ../*.c))
INC = -I../.. -I../../../include -I../../../include/cxxtest -I/usr/include
CFLAGS = -Wall -g
CC = g++

runtests: runtests.cpp ../*.c ../*.h
	$(CC) $(CFLAGS) $(INC) -o runtests runtests.cpp ../*.c

runtests.cpp: *.h
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

clean:
	rm -f runtests* *.so

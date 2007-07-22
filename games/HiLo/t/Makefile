.SUFFIXES:
.SUFFIXES: .o .c .h
OBJS := $(patsubst %.h,%.o,$(wildcard *.h))
INC = -I../.. -I../../../include -I../../../include/cxxtest
CFLAGS = -Wall -I/usr/local/include
CC = g++
TEST != ls *.h
SRC != ls ../*.c
OBS = $(TEST:.c=.o)

run: runtests
	@LD_LIBRARY_PATH=.. ./runtests

../libHiLo.so:
	make libHiLo.so -C ..

runtests: runtests.cpp $(SRC) ../libHiLo.so
	$(CC) $(CFLAGS) $(INC) -L.. -lHiLo -o runtests runtests.cpp $(SRC)

runtests.cpp: *.h
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

clean:
	rm -f runtests* *.so


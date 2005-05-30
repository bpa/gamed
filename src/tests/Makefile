CFLAGS = -Wall -I.. -I../cxxtest -DTESTING
LIBS = -L../lib -lgamed -R../lib -levent
CC = g++

run: runtests
	LD_LIBRARY_PATH=../lib ./runtests

runtests: runtests.cpp ../lib/libgamed.so utils.o
	$(CC) $(CFLAGS) $(LIBS) -o runtests runtests.cpp utils.o

utils.o: ../utils.cc
	$(CC) $(CFLAGS) -c ../utils.cc -o utils.o

runtests.cpp: *.h
	../cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

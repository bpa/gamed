CFLAGS = -Wall -I.. -I../include/cxxtest -DTESTING
LIBS = -L/usr/local/lib -L../lib -lgamed -R../lib -levent
CC = g++

run: runtests
	LD_LIBRARY_PATH=../lib ./runtests

runtests: runtests.cpp
	$(CC) $(CFLAGS) $(LIBS) -o runtests runtests.cpp utils.o

utils.o: ../utils.cc
	$(CC) $(CFLAGS) -c ../utils.cc -o utils.o

runtests.cpp: *.h
	../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

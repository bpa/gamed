CFLAGS = -Wall -I.. -I../cxxtest
LIBS = -L../lib -lgamed -R../lib
CC = g++

run: runtests
	./runtests

runtests: runtests.cpp
	$(CC) $(CFLAGS) $(LIBS) -o runtests runtests.cpp

runtests.cpp: *.h
	../cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

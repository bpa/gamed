CFLAGS = -Wall -I../../src -I../../src/cxxtest -DTESTING
CC = g++
OBJS = utils.o

run: gamed games test

test: runtests
	./runtests

games:
	cd .. && make games

gamed:
	cd ../../src && make build

runtests: runtests.cpp utils.o
	$(CC) $(CFLAGS) -o runtests runtests.cpp $(OBJS)

utils.o: ../../src/utils.cc
	$(CC) $(CFLAGS) -c ../../src/utils.cc -o utils.o

runtests.cpp: *.h
	../../src/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

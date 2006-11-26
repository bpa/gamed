INC = -I../.. -I../../../include -I../../../include/cxxtest -I../../../test -I/usr/local/include 
CFLAGS = -Wall $(INC) -g
CC = g++
TESTS != ls *.h
LIBS = -L/usr/local/lib -levent

test: runtests
	@./runtests

runtests: tests.o game.o
	$(CC) $(CFLAGS) $(LIBS) -o runtests game.o tests.o

runtests.cpp: $(TESTS)
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

tests.o: runtests.cpp
	$(CC) $(CFLAGS) -c runtests.cpp -o tests.o

game.o: ../game.cc
	$(CC) $(CFLAGS) ../game.cc -c -o game.o

clean:
	rm -f runtests runtests.cpp *.o

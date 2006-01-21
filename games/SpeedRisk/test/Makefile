CFLAGS = -Wall -I../.. -I../../../include -I../../../include/cxxtest -I../../../test -g
CC = g++
TESTS != ls *.h

test: runtests
	@./runtests

runtests: tests.o game.o borders.o
	$(CC) $(CFLAGS) -o runtests game.o borders.o tests.o

runtests.cpp: $(TESTS)
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

tests.o: runtests.cpp
	$(CC) $(CFLAGS) -c runtests.cpp -o tests.o

game.o: ../game.cc
	$(CC) $(CFLAGS) ../game.cc -c -o game.o

borders.o: ../borders.cc
	$(CC) $(CFLAGS) ../borders.cc -c -o borders.o

clean:
	rm -f runtests runtests.cpp *.o

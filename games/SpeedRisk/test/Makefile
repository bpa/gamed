CFLAGS = -Wall -I../.. -I../../../include -I../../../include/cxxtest -I../../../test -g
CC = g++
TESTS != ls *.h

test: runtests
	@./runtests

runtests: runtests.cpp game.o borders.o
	$(CC) $(CFLAGS) -o runtests runtests.cpp game.o borders.o $(TESTS)

runtests.cpp: $(TESTS)
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

game.o: ../game.cc
	$(CC) $(CFLAGS) ../game.cc -c -o game.o

borders.o: ../borders.cc
	$(CC) $(CFLAGS) ../borders.cc -c -o borders.o

clean:
	rm -f runtests runtests.cpp *.o

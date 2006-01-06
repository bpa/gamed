CFLAGS = -Wall -I../.. -I../../../include -I../../../include/cxxtest -I../../../test -g
CC = g++
TESTS != ls ../game.cc *.h

test: runtests
	@./runtests

runtests: runtests.cpp $(TESTS)
	$(CC) $(CFLAGS) -o runtests runtests.cpp $(TESTS)

runtests.cpp: $(TESTS) $(OBS)
	../../../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

clean:
	rm -f runtests runtests.cpp

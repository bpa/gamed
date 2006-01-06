CFLAGS = -Wall -I.. -I../../../include -I../../../include/cxxtest
CC = g++
TESTS != ls *.h

test: runtests
	@./runtests

runtests: runtests.cpp $(TESTS)
	$(CC) $(CFLAGS) -o runtests runtests.cpp $(TESTS)

clean:
	rm -f runtests runtests.cpp

CFLAGS = -Wall -I../src -I../include -I../include/cxxtest -I/usr/local/include -DTESTING
LIBS = -L/usr/local/lib -levent -L../lib
CC = g++
TEST != ls *.h
SRC != ls ../src/[^m]*.cc
OBS = $(SRC:.cc=.o)

run: runtests
	@LD_LIBRARY_PATH=../lib ./runtests

runtests: runtests.cpp ../lib/libgamed.so
	$(CC) $(CFLAGS) $(LIBS) -o runtests runtests.cpp -lgamed

../lib/libgamed.so: $(SRC)
	@make -C ../src

runtests.cpp: $(OBS)
	../include/cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h

clean:
	rm -f runtests.cpp *.o

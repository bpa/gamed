.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

INC = -I../.. -I../../../include -iquote..
CFLAGS = -Wall -pedantic -ansi 
LIBS = -L.. -l:Rook.so -lgtest -lgtest_main
CPP = g++

test: runtests run-tests

runtests: $(OBJS) ../Rook.so ../../test/server.o
	$(CPP) -o runtests ../../test/server.o $(LIBS) $(OBJS)

run-tests:
	./runtests

../Rook.so: ../*.c
	@make -C .. Rook.so

../../test/server.o:
	@make -C ../../test server.o

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o


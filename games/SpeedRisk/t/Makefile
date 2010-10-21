.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

CFLAGS = -Wall -pedantic -ansi 
INC = -I../.. -I../../../include -iquote..
LIBS = -L.. -l:SpeedRisk.so -lgtest -lgtest_main
CPP = g++

test: runtests run-tests

runtests: $(OBJS) ../SpeedRisk.so ../../test/server.o
	$(CPP) -o runtests ../../test/server.o $(LIBS) $(OBJS)

run-tests:
	./runtests

../SpeedRisk.so: ../*.c
	@make -C .. SpeedRisk.so

../../test/server.o:
	@make -C ../../test server.o

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o


.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

CFLAGS = -Wall -ansi 
INC = -I../.. -I../../../include -iquote.. -I$(GTEST_HOME)/include
LIBS = -pthread 
CPP = g++ -g

test: runtests run-tests

runtests: $(OBJS) ../SpeedRisk.so ../../test/server.o
	$(CPP) -o runtests ../../test/server.o $(LIBS) ../*.o $(OBJS) $(GTEST_HOME)/libgtest_main.a $(GTEST_HOME)/libgtest.a

run-tests:
	./runtests

../SpeedRisk.so: ../*.c
	@make -C .. SpeedRisk.so

../../test/server.o: ../../test/server.c
	@make -C ../../test server.o

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o


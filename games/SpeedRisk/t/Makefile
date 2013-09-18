.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

CFLAGS = -Wall -pedantic -ansi 
INC = -I../.. -I../../../include -iquote.. -I$(GTEST_HOME)/include
LIBS = -lpthread ../SpeedRisk.so
CPP = g++

test: runtests run-tests

runtests: $(OBJS) ../SpeedRisk.so ../../test/server.o
	$(CPP) -o runtests ../../test/server.o $(LIBS) $(OBJS) $(GTEST_HOME)/src/gtest_main.cc $(GTEST_HOME)/lib/.libs/libgtest.a

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


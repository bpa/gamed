.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

INC = -I../.. -I../../../include -iquote..
CFLAGS = -Wall -pedantic -ansi 
LIBS = -lgtest -lgtest_main
CPP = g++

test: runtests run-tests

runtests: $(OBJS) ../HiLo.so
	$(CPP) $(CFLAGS) $(INC) -o runtests $(OBJS) ../HiLo.so $(LIBS)

run-tests:
	./runtests

../HiLo.so: ../*.c
	@make -C .. HiLo.so

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o


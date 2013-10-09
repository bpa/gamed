.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

CFLAGS = -Wall -ansi
INC = -I../../include -iquote.. -I$(GTEST_HOME)/include
LIBS = -lpthread ../gamed.so -L /usr/local/lib -levent -ldl 
CPP = g++ 

test: runtests run-tests

runtests: $(OBJS) ../gamed.so
	$(CPP) -o runtests $(LIBS) $(OBJS) $(GTEST_HOME)/src/gtest_main.cc $(GTEST_HOME)/libgtest.a

run-tests:
	./runtests

../libgamed.so: ../*.c
	@make -C ../ gamed.so

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o

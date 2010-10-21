.SUFFIXES:
.SUFFIXES: .o .cpp
OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

CFLAGS = -Wall -pedantic -ansi
INC = -I../../include -iquote..
LIBS = -L.. -l:gamed.so -levent -ldl -lgtest -lgtest_main
CPP = g++ 

test: runtests run-tests

runtests: $(OBJS) ../gamed.so
	$(CPP) -o runtests $(LIBS) $(OBJS)

run-tests:
	./runtests

../libgamed.so: ../*.c
	@make -C ../ gamed.so

.cpp.o:
	$(CPP) $(CFLAGS) $(INC) -c $<

clean:
	rm -f runtests *.o

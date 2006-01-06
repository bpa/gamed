.SUFFIXES:
.SUFFIXES: .so .cc
SOFLAGS = -shared -Wl,-x
CFLAGS = -Wall
INC = -I../../include -I/usr/local/include -I..
CC = g++

run: sr

tests:
	@make -C test

.cc.so:
	$(CC) $(CFLAGS) $(SOFLAGS) $(INC) $< -o $@ -Wl,-soname,$@
	cp $@ ../../lib

sr: main.cc
	$(CC) $(CFLAGS) $(INC) main.cc -o sr -lcurses

clean:
	rm -f *.o *.so sr

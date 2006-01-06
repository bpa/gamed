.SUFFIXES:
.SUFFIXES: .so .cc
SOFLAGS = -shared -Wl,-x
CFLAGS = -Wall
INC = -I../../include -I/usr/local/include
CC = g++

run: tests

tests:
	@make -C test

.cc.so:
	$(CC) $(CFLAGS) $(SOFLAGS) $(INC) $< -o $@ -Wl,-soname,$@
	cp $@ ../../lib

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -f *.o *.so
	@make -C test clean

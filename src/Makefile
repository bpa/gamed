.SUFFIXES:
.SUFFIXES: .o .cc
SOURCE = server.cc
OBJS = $(SOURCE:.cc=.o)
CFLAGS = -Wall -O2 -I.
SOFLAGS = -fpic -shared -Wl,--export-dynamic
LIBS = -levent
CC = g++

all: build test

build: libs main.o
	$(CC) $(CFLAGS) -o server main.o $(OBJS) -levent

libs: lib/libgamed.so

lib/libgamed.so: $(OBJS)
	$(CC) $(SOFLAGS) $(LIBS) -o lib/libgamed.so $(OBJS)

.cc.o:
	$(CC) $(CFLAGS) -c $<

test: libs
	cd tests && make

clean:
	rm -f *.o lib/*.so

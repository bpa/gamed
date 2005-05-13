.SUFFIXES:
.SUFFIXES: .o .cc .so .test
SOURCE = server.cc
OBJS = $(SOURCE:.cc=.o)
CFLAGS = -Wall -O2 -I.
SOFLAGS = -fpic -shared -Wl,--export-dynamic

all: build test

build: libs gamed

libs: lib/libgamed.so

lib/libgamed.so: $(OBJS)
	$(CC) $(SOFLAGS) -o lib/libgamed.so $(OBJS)

.cc.o:
	$(CC) $(CFLAGS) -c $<

test:
	cd tests && make

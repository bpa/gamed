.SUFFIXES:
.SUFFIXES: .o .cc
SOURCE = server.cc db.cc
OBJS = $(SOURCE:.cc=.o)
CFLAGS = -Wall -O2 -I. -I/usr/local/include -I/home/bruce/projects/gamed/include
SOFLAGS = -fpic -shared -Wl,--export-dynamic
LIBS = -L/usr/local/lib -levent
CC = g++

all: build

build: libs main.o
	$(CC) $(CFLAGS) -o server main.o $(OBJS) $(LIBS)

libs: lib/libgamed.so

lib/libgamed.so: $(OBJS)
	$(CC) $(SOFLAGS) $(LIBS) -o lib/libgamed.so $(OBJS)

.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o lib/*.so

.SUFFIXES:
.SUFFIXES: .o .cc
SOURCE != ls [^m]*.cc
OBJS = $(SOURCE:.cc=.o)
CFLAGS = -Wall -O2 -I/usr/local/include -I../include -I../games -g
SOFLAGS = -fpic -shared -Wl,--export-dynamic
LIBS = -L/usr/local/lib -levent
CC = g++

server: $(OBJS) main.o
	$(CC) $(CFLAGS) -o server $(OBJS) main.o $(LIBS)
	$(CC) $(CFLAGS) -Wl,-x -shared -o ../lib/libgamed.so -Wl,-soname,libgamed.so $(OBJS)

.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o server

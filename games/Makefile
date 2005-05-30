.SUFFIXES:
.SUFFIXES: .o .cc
GAMES = HiLo.o
CFLAGS = -Wall -I../src
CC = g++

run: games test

test:
	cd tests && make

games: $(GAMES)

.cc.o:
	$(CC) $(CFLAGS) -c $<

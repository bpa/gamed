OBJS := $(patsubst %.c,%.o,$(wildcard ../*.c))
CFLAGS = -Wall -g -fPIC
OBJS = borders.o game.o
INC = -I.. -I../../include -I/usr/local/include
CC = gcc

run: tests libSpeedRisk.so

tests: $(OBJS)
	@make -C t
	t/runtests

libSpeedRisk.so: *.c
	ld -fPIC -shared -s -o libSpeedRisk.so $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o *.so
	@make -C t clean

OBJS := $(patsubst %.c,%.o,$(wildcard ../*.c))
CFLAGS = -Wall -g -fPIC
OBJS = borders.o game.o
INC = -I.. -I../../include -I/usr/local/include
CC = gcc

run: tests SpeedRisk.so

tests: $(OBJS)
	@make -C t
	t/runtests

SpeedRisk.so: *.c
	ld -fPIC -shared -s -o SpeedRisk.so $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o *.so
	@make -C t clean

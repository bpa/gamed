OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -Wall -fPIC
INC = -I.. -I../../include
CC = gcc -g

run: SpeedRisk.so

SpeedRisk.so: $(OBJS)
	gcc -fPIC -shared -s -o SpeedRisk.so $(OBJS)

test: 
	@make -C t

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o *.so
	@make -C t clean

borders: boards.o test_borders.c
	gcc test_borders.c -c -I../../include -I ..
	gcc test_borders.o borders.o boards.o game.o
	./a.out

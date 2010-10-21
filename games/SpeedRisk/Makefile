OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -Wall -fPIC
INC = -I.. -I../../include
CC = gcc

run: SpeedRisk.so test

SpeedRisk.so: $(OBJS)
	ld -fPIC -shared -s -o SpeedRisk.so $(OBJS)

test: 
	@make -C t

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

clean:
	rm -f *.o *.so
	@make -C t clean

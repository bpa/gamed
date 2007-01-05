.SUFFIXES:
.SUFFIXES: .o .c .h
CFLAGS = -Wall -pedantic
TESTS != t/*.h
INC = -I../../include -I/usr/local/include
CC = g++

run: HiLo.so tests

tests: runtests
	@./runtests

runtests: $(TESTS)
	@make -C t
	@mktestsuite t

clean:
	rm -f *.o runtests*
	@make -C t clean

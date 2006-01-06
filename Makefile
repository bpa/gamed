all: lib server tests game client

lib:
	mkdir -p lib

server:
	@echo "----  Making all in src  ----"
	@make -C src

tests:
	@echo "---- Making all in tests ----"
	@make -C test

game:
	@echo "---- Making all in games ----"
	@make -C games

client:
	@echo "---- Making all in clients ----"
	@make -C clients

doc:
	@echo "---- Making all in docs  ----"
	@make -C docs

clean:
	rm -rf lib
	@echo "----  Cleaning for src  ----"
	@make clean -C src
	@echo "---- Cleaning for tests ----"
	@make clean -C test
	@echo "---- Cleaning for games ----"
	@make clean -C games
	@echo "---- Cleaning for clients ----"
	@make clean -C clients

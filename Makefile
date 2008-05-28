all: lib daemon game-modules game-clients

lib:
	mkdir -p lib

daemon:
	@echo "----  Making all in server  ----"
	@make -C server

game-modules:
	@echo "---- Making all in games ----"
	@make -C games

game-clients:
	@echo "---- Making all in clients ----"
	@make -C clients

docs:
	@echo "---- Making all in docs  ----"
	@make -C docs

clean:
	rm -rf lib
	@echo "----  Cleaning for server  ----"
	@make clean -C server
	@echo "---- Cleaning for games ----"
	@make clean -C games
	@echo "---- Cleaning for clients ----"
	@make clean -C clients

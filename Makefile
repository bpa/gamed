all: _server _games _clients _docs

_server:
	@echo "----  Making all in server  ----"
	@make -C server

_Gamed:
	@echo "----  Making all in Gamed  ----"
	@make -C Gamed

_games:
	@echo "---- Making all in games ----"
	@make -C games

_clients:
	@echo "---- Making all in clients ----"
	@make -C clients

_docs:
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

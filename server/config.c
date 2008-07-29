#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <gamed/game.h>

extern GameModuleList game_module_list;
void load_game(char *path);

void read_config (const char *config) {
    char *ok;
	FILE *c = fopen(config, "r");
	if (c == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", config, strerror(errno));
		return;
	}
	for (;;) {
		ok = fgets(&buff[0], 1024, c);
		if (ok == NULL) break;
		if (strncmp("LoadGame ", &buff[0], 9) == 0) {
			load_game(&buff[9]);
		}
	};
}

void load_game(char *path) {
	char *name, *lib, *error;
	void *handle;
	Game *game;
	GameModule *module;
	lib = strtok(path, " \n");
	handle = dlopen(lib, RTLD_NOW);
	error = dlerror();
	if (error != NULL) fprintf(stderr, "%s\n", error);
	if (handle != NULL) {
		do {
			name = strtok(NULL, " \n");
			if (name != NULL) {
				game = (Game *)dlsym(handle, name);
				module = (GameModule*)malloc(sizeof(GameModule));
				error = dlerror();
				if (error != NULL) fprintf(stderr, "%s\n", error);
				if (game != NULL) {
					bzero(module, sizeof(GameModule));
					memcpy(module, game, sizeof(Game));
					LIST_INSERT_HEAD(&game_module_list, module, game_entry);
				}
			}
		} while (name != NULL);
	}
}
	

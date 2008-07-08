#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <gamed/game.h>

extern GameList game_list;
void load_game(char *path);

void read_config (const char *config) {
	ssize_t read;
	size_t len;
	char *line = NULL;
	FILE *c = fopen(config, "r");
	if (c == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", config, strerror(errno));
		return;
	}
	for (;;) {
		read = getline(&line, &len, c);
		if (read == -1) break;
		if (strncmp("LoadGame ", line, 9) == 0) {
			load_game(&line[9]);
		}
	};
	free(line);
}

void load_game(char *path) {
	char *name, *lib, *error;
	void *handle;
	Game *game;
	lib = strtok(path, " \n");
	handle = dlopen(lib, RTLD_NOW);
	error = dlerror();
	if (error != NULL) fprintf(stderr, "%s\n", error);
	if (handle != NULL) {
		do {
			name = strtok(NULL, " \n");
			if (name != NULL) {
				game = (Game *)dlsym(handle, name);
				error = dlerror();
				if (error != NULL) fprintf(stderr, "%s\n", error);
				if (game != NULL) {
					LIST_INSERT_HEAD(&game_list, game, game);
				}
			}
		} while (name != NULL);
	}
}
	

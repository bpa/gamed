#define _GNU_SOURCE
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <gamed/game.h>

extern GameModuleList game_module_list;
void load_game(char *path, char* root);
#define MAX_LINE_LEN 1024

void read_config (const char *config) {
    size_t len;
    char *root, *ok, *line;
	FILE *c = fopen(config, "r");
    line = (char*)malloc(MAX_LINE_LEN);
    root = "/usr/local/gamed";
	if (c == NULL) {
		fprintf(stderr, "Can't open %s: %s\n", config, strerror(errno));
		return;
	}
	for (;;) {
		ok = fgets(&line[0], 1024, c);
		if (ok == NULL) break;
		if (strncmp("ServerRoot ", &line[0], 11) == 0) {
			len = strlen(&line[11])+1;
            root = realloc(root, len);
            memcpy(root, &line[11], len);
            root[len] = '\0';
		}
		if (strncmp("LoadGame ", &line[0], 9) == 0) {
			load_game(&line[9], root);
		}
	};
}

void load_game(char *path, char* root) {
	char *name, *lib, *error, *fullpath, *sroot;
	void *handle;
	Game *game;
	GameModule *module;
	sroot = strtok(root, "\n");
	lib   = strtok(path, " \n");
    fullpath = malloc(strlen(lib) + strlen(root) + 1);
    strcpy(fullpath, root);
    strcat(fullpath, "/");
    strcat(fullpath, path);
	handle = dlopen(fullpath, RTLD_NOW);
	error = dlerror();
	if (error != NULL) fprintf(stderr, "%s\n", error);
	if (handle != NULL) {
		do {
			name = strtok(NULL, " \n");
			if (name != NULL) {
                printf("Loading %s\n", name);
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
	

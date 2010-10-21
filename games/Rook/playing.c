#include "rook.h"

void start_playing (GameInstance *g, const Server *s);
void handle_playing (GameInstance *g, const Server *s, Player *p, const char *req, int len);

State PLAYING = { start_playing, handle_playing, NULL, NULL };

void start_playing (GameInstance *g, const Server *s) {}

void handle_playing (GameInstance *g, const Server *s, Player *p, const char *req, int len) {
}

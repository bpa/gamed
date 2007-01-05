#include <sys/types.h>
#include <sys/param.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <gamed/server.h>

extern Server server_funcs;
extern GameInstanceList game_list;
extern ChatInstanceList chat_list;
extern void handle_network_event(int sock, short event, void *args);
extern void handle_network_event(int sock, short event, void *args);
extern long get_random(long max);

static int argc;
static char *argv[5];

static void parse_args(int len) {
    char *ind;
    argc = 0;
    memset(argv, 0, sizeof(argv));
    ind = &buff[0];
    while (argc < 5 && len > 0 && * ind != '\0') {
        if (isspace(*ind)) {
            *ind++ = '\0';
            len--;
        }
        else {
            argv[argc++] = ind++;
            while(!isspace(*ind) && len > 0) {
                ind++;
                len--;
            }
        }
    }
}

void handle_command(Client *client, int len) {
    Player *plr;
    GameInstance *instance;
    u_int16_t buff_len;
    u_int16_t *ptr_len = (u_int16_t*)&buff[2];
    parse_args(len);
    if (argc == 2 && strncmp("/name", argv[0], 5) == 0) {
        strncpy(client->player.name, argv[1], 16);
        client->player.name[15] = '\0';
        client->player.name[len-6] = '\0';
        memset(&buff[0], 0, 4);
        if (client->game != NULL) {
            buff_len = sprintf(&buff[4], "rename:%i:%s",
                 client->player.in_game_id, &client->player.name[0]);
            *ptr_len = htons(buff_len);
            server_funcs.tell_all(client->game, &buff[0], buff_len+4);
        }
        return;
    }
    if (argc == 1 && strncmp("/players", argv[0], 8) == 0) {
        if (client->game != NULL) {
            memset(&buff[0], 0, 4);
            buff_len = 4;
            buff_len += sprintf(&buff[4], "players");
            LIST_FOREACH(plr, &client->game->players, player) {
                buff_len += sprintf(&buff[buff_len], ":%i:%s",
                     plr->in_game_id, &plr->name[0]);
            }
            buff_len -= 4;
            *ptr_len = htons(buff_len);
            server_funcs.tell_player(&client->player, &buff[0], buff_len+4);
        }
        else {
            memset(&buff[0], 0, 4);
            buff_len = 4;
            buff_len += sprintf(&buff[4], "players:0");
            server_funcs.tell_player(&client->player, &buff[0], buff_len);
        }
        return;
    }
    if (argc == 3 && strncmp("/create", argv[0], 7) == 0) {
        /***********************************
         * Right now, there is only one game
         * Ignoring second argument
         ***********************************/

        instance = (void*)malloc(sizeof(GameInstance));
        memset(instance, 0, sizeof(GameInstance));
        strncpy(instance->name, argv[2], 32);
        instance->name[31] = '\0';
        instance->game = available_games[1];
        LIST_INSERT_HEAD(&game_list, instance, game_instance);
        LIST_INIT(&instance->players);
        instance->playing = 0;
        instance->functions = &server_funcs;
        instance->game->game_init(instance);
        if (instance->game->player_join(instance, &client->player)) {
            syslog(LOG_INFO, "%s joined", client->player.name);
            client->game = instance;
        }
        return;
    }
    if (argc == 1 && strncmp("/list", argv[0], 5) == 0) {
        memset(&buff[0], 0, 4);
        buff_len = 0;
        LIST_FOREACH(instance, &game_list, game_instance) {
            buff_len += sprintf(&buff[buff_len], "%s\n", &instance->name[0]);
        }
        server_funcs.tell_player(&client->player, &buff[0], buff_len);
        return;
    }
    if (argc == 2 && strncmp("/join", argv[0], 5) == 0) {
        instance = LIST_FIRST(&game_list);
        while (instance != NULL) {
            if (strncmp(instance->name, argv[1], 32) == 0) {
                break;
            }
            instance = LIST_NEXT(instance, game_instance);
        }

        if (instance != NULL) {
            if (instance->game->player_join(instance, &client->player)) {
                syslog(LOG_INFO, "%s joined", client->player.name);
            }
        }
        return;
    }
    if (argc == 1 && strncmp("/quit", argv[0], 5) == 0) {
        if (client->game == NULL) {
            server_funcs.tell_player(&client->player, "You aren't in a game\n",0);
        }
        else {
            client->game->game->player_quit(client->game, &client->player);
            if (client->game->playing == 0) {
                /*TODO refactor to remove this duplication of code */
                /*TODO check to see if timer was added before deleting */
                event_del(&client->game->timer);
                free(client->game->data);
                free(client->game);
            }
            LIST_REMOVE(client->game, game_instance);
            client->game = NULL;
        }
    }
    server_funcs.tell_player(&client->player, "Invalid command\n",0);
}

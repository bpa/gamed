use Gamed::Game;

class Gamed::Game::HiLo is Gamed::Game;

has Int $.number is rw;
has Int $.guesses is rw;

submethod BUILD {
    $.name    = 'HiLo';
    $.version = '0.1';
}

method player_joined( $server, $client ) {
    $.number  = 100.rand.ceiling;
    $.guesses = 0;
}

method handle_message ( Gamed::Server $server, Gamed::Client $client, %msg ) {
    $.guesses++;
    my %resp = { guesses => $.guesses };
    if ( %msg<guess> == $.number ) {
        %resp<result> = 'correct';
        $.number = 100.rand.ceiling;
    }
    else {
        %resp<result> =
             %msg<guess> < $.number
          ?? 'too low'
          !! 'too high';
    }
    $server.send( %resp, $client );
}

#void player_join (GameInstance *g, const Server *s, Player *p) {
#    g->accepting_players = 0;
#    strcpy(g->status, "Guessing");
#}
#
#void player_quit (GameInstance *g, const Server *s, Player *p) {
#    s->game_over(g);
#}
#
#void handle_request (GameInstance *g, const Server *s, Player *p, const char *req, int len) {
#    Command cmd;
#    Command *guess = (Command *)req;
#    HiLoData *game = (HiLoData*)g->data;
#    game->guesses++;
#    cmd.subcmd = game->guesses;
#    switch(guess->command) {
#        case HILO_GIVEUP:
#            s->game_over(g);
#            break;
#        default:
#            if (guess->subcmd == game->number) {
#                cmd.command = HILO_CORRECT;
#                game->number = s->random(100);
#                game->guesses = 0;
#                s->tell_player(p, (char*)&cmd, 2);
#            }
#            else if (guess->subcmd > game->number) {
#                cmd.command = HILO_TOO_HIGH;
#                s->tell_player(p, (char*)&cmd, 2);
#            }
#            else {
#                cmd.command = HILO_TOO_LOW;
#                s->tell_player(p, (char*)&cmd, 2);
#            }
#    }
#}
#

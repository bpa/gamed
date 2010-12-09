class Gamed::Game;

use Gamed::Client;
use Gamed::Server;

has Num $.version is rw;
has Str $.name is rw;
has Bool $.accepting_players is rw;
has Bool $.in_progress is rw;
has %!clients is rw;

submethod BUILD {
    $.accepting_players = True;
    $.in_progress = True;
}

method game_init (Gamed::Server $server ) { ... }

multi method handle_message (Gamed::Server $server, Gamed::Client $client, %msg) { ... }

method player_join (Gamed::Server $server, Gamed::Client $client) { ... }

method player_quit (Gamed::Server $server, Gamed::Client $client) { ... }

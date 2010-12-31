class Gamed::Game;

use Gamed::Player;
use Gamed::Server;

has Num $.version is rw;
has Str $.name is rw;
has Bool $.accepting_players is rw;
has Bool $.in_progress is rw;
has %!players is rw;

submethod BUILD {
    $.accepting_players = True;
    $.in_progress = True;
}

method game_init (Gamed::Server $server ) { ... }

multi method handle_message (Gamed::Server $server, Gamed::Player $player, %msg) { ... }

method player_join (Gamed::Server $server, Gamed::Player $player) { ... }

method player_quit (Gamed::Server $server, Gamed::Player $player) { ... }

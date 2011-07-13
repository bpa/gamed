class Gamed::Game;

use Gamed::Player;

has Num $.version is rw;
has Str $.name is rw;
has Bool $.accepting_players is rw;
has Bool $.in_progress is rw;
has %!players is rw;

submethod BUILD {
    $.accepting_players = True;
    $.in_progress = True;
}

method game_init () { ... }

multi method handle_message (Gamed::Player $player, %msg) { ... }

method player_join (Gamed::Player $player) { ... }

method player_quit (Gamed::Player $player) { ... }

multi method send ( %msg ) {
	%!players.values>>.send(%msg);
}

multi method send ( %msg, $player ) {
	$player.send(%msg);
}

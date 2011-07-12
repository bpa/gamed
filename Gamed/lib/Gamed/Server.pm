class Gamed::Server;

use Gamed::Player;

has $!sock;
has Gamed::Player @.players;

method send ( %msg ) {
	
}

method send ( %msg, $player ) {}


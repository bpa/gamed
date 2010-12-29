class Gamed::Server;

use Gamed::Player;

has Gamed::Player @.players;

multi method send ( %msg ) { ... }

multi method send ( %msg, $player ) { ... }


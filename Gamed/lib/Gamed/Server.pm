class Gamed::Server;

use Gamed::Player;

has $!sock;
has Gamed::Player @.players;

submethod BUILD () {
	$!sock = IO::Socket::INET.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        );
}

method send ( %msg ) {
	
}

method send ( %msg, $player ) {}


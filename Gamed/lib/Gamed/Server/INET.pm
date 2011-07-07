use Gamed::Server;

class Gamed::Server::INET is Gamed::Server;

has $!sock;
has $!filter;

submethod BUILD () {
	$!sock = IO::Socket::INET.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        );
}

multi method send ( %msg ) {
	
}

multi method send ( %msg, $player ) { ... }


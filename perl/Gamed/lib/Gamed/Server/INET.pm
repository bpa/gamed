class Gamed::Server::INET;

has $!sock;
has $!filter;

submethod BUILD () {
	$!sock = IO::Socket::INET.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        );
}

method accept() {
	return $!sock.accept();
}

multi method send ( %msg ) {
	
}

multi method send ( %msg, $player ) { ... }


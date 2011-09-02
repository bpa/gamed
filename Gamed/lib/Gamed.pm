use Gamed::Server::Commands;

class Gamed does Gamed::Server::Commands;

use JSON::Tiny;

has %!games;
has @!players;

method run() {
	my $sock = IO::Socket::INET.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        )
		but role {
			method ready () { 
					fail("Not connected") unless $!PIO; 
					return $!PIO.poll(1, 0, 0); 
			}
		};
	loop {
		if $sock.ready() {
			my $c = $sock.accept();
			@!players.push(Gamed::Player.new(:sock($c)));
		}
		for @!players -> $p {
			try {
				my $msg = from-json($p.get_input);
				if $msg.defined {
					handle_message($p, $msg)
					|| $p.game.handle_message($p, $msg);
				}
			}
		}
	}
}

class Gamed;

use JSON::Tiny;
use Gamed::Commands;

method run() {
	my @!players;
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
		}
	loop {
		if $sock.ready() {
			my $c = $sock.accept();
			@!players.push(Gamed::Player.new(:sock($c)));
		}
		for @!players -> $p {
			try {
				my $msg = $p.get_input;
				if $msg.defined {
					$p.game.handle_message($p, from-json($msg));
				}
			}
		}
	}
}

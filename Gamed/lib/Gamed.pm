#use Gamed::Server::Commands;

class Gamed;# does Gamed::Server::Commands;

use JSON::Tiny;
use WebSocket;

has %!games;
has @!players;

method run() {
	my $sock = WebSocket.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        );
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

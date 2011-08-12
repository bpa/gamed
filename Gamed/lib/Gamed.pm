class Gamed;

use Gamed::Game::HiLo;
use JSON::Tiny;

#has $!games;
#has $!commands;
#has $!players;

method run() {
	my $sock = IO::Socket::INET.new(
            :localhost('localhost'),
            :localport(3939),
            :listen,
        );
	loop {
		my $c = $sock.accept();
		my $game = Gamed::Game::HiLo.new;
		my $player = Gamed::Player.new(:sock($c));
		$game.player_join($player);
		while my $r = $c.recv() {
			say $r;
			$game.handle_message($player, from-json($r));
		}
	}
}

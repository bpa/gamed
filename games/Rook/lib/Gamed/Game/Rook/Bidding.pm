use Gamed::State;

class Gamed::Game::Rook::Bidding is Gamed::State;

submethod BUILD () { 
	$.name = 'bidding';
}

method enter_state ( Gamed::Server $server, Gamed::Game $rook ) {
	$rook.accepting_players = False;
	$server.send( { 'state' => 'bidding' } );
	$rook.bid = 95;
	$rook.bidder = Any;
	$rook.passed = 0;
	for $rook.seats.values {
		$_<passed> = False;
	}
	$rook.dealer = $rook.seats{$rook.dealer}<next>;
	$rook.current_player = $rook.seats{$rook.dealer}<next>;
}

method handle_message ( Gamed::Server $server, Gamed::Game $rook, Gamed::Player $player, %msg ) {
	if $player.game<seat> !~~ $rook.current_player {
		$server.send( { event => 'error', msg => 'not your turn' }, $player );
		return;
	}

	given %msg<bid> {
		when 'pass' {
			$server.send( { event => 'bid', player => $rook.current_player, bid => 'pass' } );
			$rook.seats{$player.game<seat>}<passed> = True;
			$rook.passed++;
			self.next_player($rook);
			if $rook.passed == 3 {
				$rook.bid = 100 if $rook.bid < 100;
				$rook.bidder = $rook.current_player;
				$rook.change_state( 'picking', $server );
			}
		}
		when $_ <= $rook.bid {
			$server.send( { event => 'error', msg => 'bid too low' }, $player );
		}
		when $_ > 200 {
			$server.send( { event => 'error', msg => 'bid too high' }, $player );
		}
		when $_ % 5 != 0 {
			$server.send( { event => 'error', msg => 'bid invalid' }, $player );
		}
		default {
			$rook.bid = %msg<bid>;
			$rook.bidder = $player.game<seat>;
			$server.send( { event => 'bid', player => $rook.bidder, bid => $rook.bid } );
			if $rook.bid == 200 {
				$rook.change_state( 'picking', $server );
			}
			else {
				self.next_player($rook);
			}
		}
	}
}

method next_player ( $rook ) {
	my $current = $rook.current_player;
	for 1 .. 3 {
		$current = $rook.seats{$current}<next>;
		last unless $rook.seats{$current}<passed>;
	}
	$rook.current_player = $current;
}

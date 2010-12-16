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
}

method handle_message ( Gamed::Server $server, Gamed::Game $rook, Gamed::Client $client, %msg ) {
	if $client.name !~~ $rook.current_player {
		$server.send( { event => 'error', msg => 'Not your turn' }, $client );
		return;
	}

	given %msg<bid> {
		when 'pass' {
			$rook.current_player = self.next_player($rook);
			$rook.seats{$client.game<seat>}<passed> = True;
			$server.send( { event => 'bid', player => $rook.bidder, bid => 'pass' } );
			$rook.passed++;
			self.next_player($rook);
			if $rook.passed == 3 {
				$rook.bidder = $rook.current_player;
				$rook.change_state( 'picking', $server );
			}
		}
		when $_ <= $rook.bid {
			$server.send( { event => 'error', msg => 'bid too low' }, $client );
		}
		when $_ > 200 {
			$server.send( { event => 'error', msg => 'bid too high' }, $client );
		}
		when $_ % 5 != 0 {
			$server.send( { event => 'error', msg => 'bid invalid' }, $client );
		}
		default {
			$rook.bid = %msg<bid>;
			$rook.bidder = $client.game<seat>;
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

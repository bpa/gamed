use Gamed::State;

class Gamed::Game::Rook::Bidding is Gamed::State;

submethod BUILD () { 
	$.name = 'bidding';
}

method enter_state ( Gamed::Game $rook ) {
	$rook.accepting_players = False;
	$rook.send( { 'state' => 'bidding' } );
	$rook.deck.add($_<hand>.draw(*)) for $rook.seats.values;
	$rook.deck.shuffle;
	$rook.bid = 95;
	$rook.bidder = Any;
	$rook.passed = 0;
	for $rook.seats.values -> $s {
		$s<passed> = False;
		$s<hand>.add($rook.deck.draw(10));
		$rook.send( { action => 'deal', cards => $s<hand> }, $s<player> )
			if $s<player>.defined;
	}
	$rook.dealer = $rook.seats{$rook.dealer}<next>;
	$rook.current_player = $rook.seats{$rook.dealer}<next>;
}

method handle_message ( Gamed::Game $rook, Gamed::Player $player, %msg ) {
	if $player.game<seat> !~~ $rook.current_player {
		$rook.send( { event => 'error', msg => 'not your turn' }, $player );
		return;
	}

	given %msg<bid> {
		when 'pass' {
			$rook.send( { event => 'bid', player => $rook.current_player, bid => 'pass' } );
			$rook.seats{$player.game<seat>}<passed> = True;
			$rook.passed++;
			self.next_player($rook);
			if $rook.passed == 3 {
				$rook.bid = 100 if $rook.bid < 100;
				$rook.bidder = $rook.current_player;
				$rook.change_state( 'picking');
			}
		}
		when $_ <= $rook.bid {
			$player.send( { event => 'error', msg => 'bid too low' } );
		}
		when $_ > 200 {
			$player.send( { event => 'error', msg => 'bid too high' } );
		}
		when $_ % 5 != 0 {
			$player.send( { event => 'error', msg => 'bid invalid' } );
		}
		default {
			$rook.bid = %msg<bid>;
			$rook.bidder = $player.game<seat>;
			$rook.send( { event => 'bid', player => $rook.bidder, bid => $rook.bid } );
			if $rook.bid == 200 {
				$rook.change_state('picking');
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

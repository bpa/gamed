use Gamed::State;

class Gamed::Game::Rook::Joining is Gamed::State;

submethod BUILD () {
    $.name = 'joining';
}

method enter_state ( Gamed::Game $game ) {
    $game.send( { 'state' => 'joining' } );
}

multi method handle_message (Gamed::Game $rook, Gamed::Player $player, %msg) {
	my $seat = $rook.seats{$player.game<seat>};
	given %msg<action> {
		when 'ready' {
			$seat<ready> = True;
			$rook.send( { action => 'ready', seat => $player.game<seat> } );
		}
		when 'not ready' {
			$seat<ready> = False;
			$rook.send( { action => 'not ready', seat => $player.game<seat> } );
		}
	}
	$rook.change_state('bidding') if [&&] $rook.seats.values»<ready>;
}


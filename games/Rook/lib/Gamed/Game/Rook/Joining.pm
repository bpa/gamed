use Gamed::State;

class Gamed::Game::Rook::Joining is Gamed::State;

submethod BUILD () {
    $.name = 'joining';
}

method enter_state ( Gamed::Server $server, Gamed::Game $game ) {
    $server.send( { 'state' => 'joining' } );
}

multi method handle_message (Gamed::Server $server, Gamed::Game $rook, Gamed::Player $player, %msg) {
	my $seat = $rook.seats{$player.game<seat>};
	given %msg<action> {
		when 'ready' {
			$seat<ready> = True;
			$server.send( { action => 'ready', seat => $player.game<seat> } );
		}
		when 'not ready' {
			$seat<ready> = False;
			$server.send( { action => 'not ready', seat => $player.game<seat> } );
		}
	}
	$rook.change_state('bidding', $server) if [&&] $rook.seats.values»<ready>;
}


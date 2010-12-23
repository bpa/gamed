use Gamed::State;

class Gamed::Game::Rook::Joining is Gamed::State;

submethod BUILD () {
    $.name = 'joining';
}

method enter_state ( Gamed::Server $server, Gamed::Game $game ) {
    $server.send( { 'state' => 'joining' } );
}

multi method handle_message (Gamed::Server $server, Gamed::Game $rook, Gamed::Client $client, %msg) {
	my $seat = $rook.seats{$client.game<seat>};
	given %msg<action> {
		when 'ready' {
			$seat<ready> = True;
			$server.send( { action => 'ready', seat => $client.game<seat> } );
		}
		when 'not ready' {
			$seat<ready> = False;
			$server.send( { action => 'not ready', seat => $client.game<seat> } );
		}
	}
	$rook.change_state('bidding', $server) if [&&] $rook.seats.values»<ready>;
}


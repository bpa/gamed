use Gamed::State;

class Gamed::Game::Color::State is Gamed::State;

has $.next_state is rw;

method enter_state ( Gamed::Server $server, Gamed::Game $game ) {
	$server.send( { enter => $.name } );
}

multi method handle_message ( Gamed::Server $server, Gamed::Game $game, Gamed::Client $client, %msg ) {
    $server.send( { color => $.name }, $client );
    $game.change_state( $.next_state, $server ) if %msg<color> eq $.next_state;
}

method leave_state ( Gamed::Server $server, Gamed::Game $game ) {
	$server.send( { leave => $.name } );
}

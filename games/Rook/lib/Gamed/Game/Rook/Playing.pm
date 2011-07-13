use Gamed::State;

class Gamed::Game::Rook::Playing is Gamed::State;

submethod BUILD () {
	$.name = 'playing';
}

method enter_state ( Gamed::Game $game ) {
	$game.send( { 'state' => 'playing' } );
	$game.round = {
		ns_points => 0,
		ew_points => 0,
		tricks => {},
	};
}


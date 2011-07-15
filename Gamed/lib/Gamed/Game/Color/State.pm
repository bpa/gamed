role Gamed::Game::Color::State is Gamed::State;

use Gamed::Game;
use Gamed::Player;

has $.next_state is rw;

method enter_state ( Gamed::Game $game ) {
	$game.send( { enter => $.name } );
}

multi method handle_message ( Gamed::Game $game, Gamed::Player $player, %msg ) {
    $player.send( { color => $.name } );
    $game.change_state( $.next_state ) if %msg<color> eq $.next_state;
}

method leave_state ( Gamed::Game $game ) {
	$game.send( { leave => $.name } );
}

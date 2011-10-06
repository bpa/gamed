#use Gamed::Game;
use Gamed::Role::StateMachine;

class Gamed::Game::Color is Gamed::Game does Gamed::Role::StateMachine;

#use Gamed::Game::Color::Red;
#use Gamed::Game::Color::Blue;

submethod BUILD () {
	$.name    = 'Color';
	$.version = '0.1';

	self.add_states(
		Gamed::Game::Color::Red.new,
		Gamed::Game::Color::Blue.new);
    self.change_state('blue');
}

method player_joined ( Gamed::Player $player ) {
	%!players<p> = $player;
    $.accepting_players = False;
    $.in_progress = True;
}

method player_quit ( Gamed::Player $player ) {
    $.accepting_players = True;
    $.in_progress = False;
}

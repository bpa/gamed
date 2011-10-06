#use Gamed::Game::Color::State;

class Gamed::Game::Color::Red is Gamed::Game::Color::State;

submethod BUILD () {
    $.name = 'red';
	$.next_state = 'blue';
}

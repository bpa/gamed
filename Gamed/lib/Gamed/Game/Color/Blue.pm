#use Gamed::Game::Color::State;

class Gamed::Game::Color::Blue is Gamed::Game::Color::State;

submethod BUILD {
	$.name = 'blue';
	$.next_state = 'red';
}

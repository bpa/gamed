use Gamed::Game;

class Gamed::Game::War is Gamed::Game;

has Int @!deck;

submethod BUILD() {
	$.name        = 'War';
    $.version     = '0.1';
	$.max_players = 2;
	for 1 .. 3 {
		for 1 .. 9 -> $val {
			push @!deck, $val;
		}
	}
}

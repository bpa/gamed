class Gamed::Game::War::Player;

use Gamed::Game::Util::Deck;

has $deck;

submethod BUILD {
	$deck = Gamed::Game::Util::Deck.new;
}


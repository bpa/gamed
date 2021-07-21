class Gamed::Game::War::Player;

use Gamed::Util::Deck;

has $deck;

submethod BUILD {
	$deck = Gamed::Util::Deck.new;
}


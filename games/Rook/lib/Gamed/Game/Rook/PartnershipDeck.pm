use Gamed::Util::Deck;

class Gamed::Game::Rook::PartnershipDeck is Gamed::Util::Deck;

submethod BUILD {
	for <black green red yellow> -> $color {
    	for 1, 5 .. 14 -> $value {
        	@!cards.push(Gamed::Util::Card.new( suit => $color, value => $value ));
    	}
	}
    @!cards.push(Gamed::Util::Card.new( suit => 'rook', value => 15 ));
}

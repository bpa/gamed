class Gamed::Game::Rook::PartnershipDeck is Gamed::Game::Util::Deck;

submethod BUILD {
	for <black green red yellow> -> $color {
    	for 5 .. 14 -> $value {
        	@!cards.push(Gamed::Game::Util::Card.new( suit => $color, value => $value ));
    	}
	}
    @!cards.push(Gamed::Game::Util::Card( suit => 'rook', value => 15 ));
}

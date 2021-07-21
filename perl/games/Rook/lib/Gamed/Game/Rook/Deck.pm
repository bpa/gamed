use Gamed::Util::Deck;

class Gamed::Game::Rook::Deck is Gamed::Util::Deck;

submethod BUILD {
	for <black green red yellow> -> $color {
    	for 1 .. 14 -> $value {
        	@!cards.push(Gamed::Util::Card.new( suit => $color, value => $value ));
    	}
	}
    @!cards.push(Gamed::Util::Card.new( suit => 'rook', value => 15 ));
}

method has_suit ( Str $suit, Str $trump ) {
	if ($suit ~~ $trump) {
		return any('rook', $suit) ~~ any(@!cards».suit);
	}
	else {
		return $suit ~~ any(@!cards».suit);
	}
	return;
}

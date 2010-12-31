class Gamed::Util::Deck;

use Gamed::Util::Card;

has Gamed::Util::Card @!cards is rw;

multi method add (Gamed::Util::Card $card) {
	@!cards.push($card);
}

multi method add (@card) {
	@!cards.push(@card);
}

multi method draw () {
	return @!cards.pop;
}

multi method draw (Int $num) {
	my @hand;
    for 1 .. $num {
        @hand.unshift(@!cards.pop);
    }
    return @hand;
}

multi method draw (Whatever) {
	my @hand = @!cards;
    @!cards = ();
    return @hand;
}

method elems () {
    return @!cards.elems;
}

method Numeric () {
	return +@!cards;
}

method has_card (Gamed::Util::Card $card) {
	return $card ~~ @!cards;
}

method shuffle () {
    @!cards .= pick(*);
    return;
}

multi method pick () {
    my $card = @!cards.pick;
    self._remove($card);
    return $card;
}

multi method pick (Int $num) {
    my @hand;
    for 1 .. $num {
        @hand.push(self.pick);
    }
    return @hand;
}

multi method pick (Whatever) {
	my @hand = @!cards.pick(*);
    @!cards = ();
    return @hand;
}

method _remove($card) {
    for @!cards.keys -> $k {
        if $card ~~ @!cards[$k] {
            @!cards.splice($k, 1);
            return;
        }
    }
}

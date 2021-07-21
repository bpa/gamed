class Gamed::Util::Card;

has Int $.value;
has Str $.suit;

method Str() {
	return $.suit.substr(0,1) ~ $.value;
}

method ACCEPTS (Gamed::Util::Card $c) {
	return $.value == $c.value
		&& $.suit eq $c.suit;
}

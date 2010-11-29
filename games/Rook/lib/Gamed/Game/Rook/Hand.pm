use Gamed::Game::Util::Deck;

class Gamed::Game::Rook::Hand is Gamed::Game::Util::Deck;

method is_valid_play( Str $lead, Str $trump, Gamed::Game::Util::Card $card ) {
	return False unless $card ~~ any(@!cards);

	my $have_lead = False;
	if ($lead eq $trump) {
		return True if $card.suit eq 'rook';
		$have_lead ||= 'rook' ~~ any(@!cards».suit);
	}
	$have_lead ||= $lead ~~ any(@!cards».suit);

	return $card.suit ~~ $lead if $have_lead;
	return True;
}

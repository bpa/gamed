use Test;
use Gamed::Game::Rook::Deck;
use Gamed::Game::Rook::PartnershipDeck;

my $deck = Gamed::Game::Rook::Deck;
$deck.perl.say;
is( $deck.elems, 57 );

$deck = Gamed::Game::Rook::PartnershipDeck;
is( $deck.elems, 45 );

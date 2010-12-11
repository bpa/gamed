use Test;
use Gamed::Game::Rook::Deck;
use Gamed::Game::Rook::PartnershipDeck;

my $deck = Gamed::Game::Rook::Deck.new;
is( $deck.elems, 57 );

$deck = Gamed::Game::Rook::PartnershipDeck.new;
is( $deck.elems, 45 );

done_testing;

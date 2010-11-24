use Test;

my $deck = Gamed::Game::Rook::Deck;
is( $deck.cards.elems, 57 );
my $card = $deck.draw;
isa_ok( $card, 'Gamed::Game::Rook::Card' );
is( $deck.cards.elems, 56 );

$deck = Gamed::Game::Rook::PartnershipDeck;
is( $deck.cards.elems, 45 );

my $hand = Gamed::Game::Rook::Hand;
is( $hand.cards, 0 );

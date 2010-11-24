package Gamed::Game::Rook::PartnershipDeck;

use Moose;
extends 'Gamed::Game::Rook::Deck';

sub build_deck {
    my $self = shift;
    my @cards;
    for my $color (qw/ black green red yellow /) {
        for ( 1, 5 .. 14 ) {
            push @cards, new Gamed::Game::Rook::Card( suit => $color, value => $_ );
        }
    }
    push @cards, new Gamed::Game::Rook::Card( suit => 'rook', value => 15 );
    return \@cards;
}

no Moose;
__PACKAGE__->meta->make_immutable;

1;

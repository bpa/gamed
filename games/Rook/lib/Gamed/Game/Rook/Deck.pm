package Gamed::Game::Rook::Deck;

use Moose;
use Gamed::Game::Rook::Card;

has cards => (
    is      => 'ro',
    isa     => 'ArrayRef[Gamed::Game::Rook::Card]',
    builder => 'build_deck',
    writer  => '_set_cards',
);

sub add {
    my $self = shift;
    for ( @_ ) {
        push( @{ $self->cards }, $_ ) if UNIVERSAL::DOES( $_, 'Gamed::Game::Rook::Card' );
    }
}

sub has_card {
    my ( $self, $card ) = @_;
    for ( @{ $self->cards } ) {
        return 1 if $_ == $card;
    }
    return;
}

sub build_deck {
    my $self = shift;
    my @cards;
    for my $color (qw/ black green red yellow /) {
        for ( 1 .. 14 ) {
            push @cards, new Gamed::Game::Rook::Card( suit => $color, value => $_ );
        }
    }
    push @cards, new Gamed::Game::Rook::Card( suit => 'rook', value => 15 );
    return \@cards;
}

sub has_suit {
    my ( $self, $suit, $trump ) = @_;
    for ( @{ $self->cards } ) {
        return 1 if $_->suit eq $suit;
        return 1 if $_->suit eq 'rook' && $suit eq $trump;
    }
    return;
}

sub draw {
    my $self = shift;
    pop @{ $self->cards };
}

# Fisher-Yates Shuffle
sub shuffle {
    my $array = shift->cards;
    my $i;
    for ( $i = @$array ; --$i ; ) {
        my $j = int rand( $i + 1 );
        next if $i == $j;
        @$array[ $i, $j ] = @$array[ $j, $i ];
    }
}

no Moose;
__PACKAGE__->meta->make_immutable;

1;

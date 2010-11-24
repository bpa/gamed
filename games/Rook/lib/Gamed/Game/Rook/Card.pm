package Gamed::Game::Rook::Card;

use Moose;
use Moose::Util::TypeConstraints;

enum 'RookSuit' => qw/ red green yellow black rook /;

use overload '""' => 'to_string';
use overload '==' => 'is_equal';

sub to_string {
    my $self = shift;
    return $self->suit . ' ' . $self->value;
}

sub is_equal {
    my ( $self, $card ) = @_;
    return $self->suit eq $card->suit
      && $self->value == $card->value;
}

has suit => (
    is       => 'ro',
    isa      => 'RookSuit',
    required => 1,
);

has value => (
    is       => 'ro',
    isa      => 'Int',
    required => 1,
);

sub points {
    my $rank = shift->rank;
    return
        $rank == 5  ? 5
      : $rank == 10 ? 10
      : $rank == 14 ? 10
      : $rank == 1  ? 15
      : $rank == 15 ? 20
      :               0;
}

sub is_valid_play {
    my ( $self, $lead, $trump, $hand ) = @_;
    return unless $hand->has_card($self);
    return 1 unless $lead;
    return 1 if $self->suit eq $lead;
    return 1 if $lead eq $trump && $self->suit eq 'rook';
    return 1 unless $hand->has_suit( $lead, $trump );
    return;
}

no Moose;
__PACKAGE__->meta->make_immutable;
1;

package Gamed::Game::Rook;

use Moose;
with 'Gamed::Role::Game';
with 'Gamed::Role::StateMachine';
use Gamed::Game::Rook::Deck;

use constant name        => 'Rook';
use constant version     => '0.1';
use constant start_state => 'joining';

has deck => (
    is      => 'ro',
    isa     => 'Gamed::Game::Rook::Deck',
    default => sub { new Gamed::Game::Rook::Deck },
);

sub build_deck {
}

no Moose;
__PACKAGE__->meta->make_immutable;

1;

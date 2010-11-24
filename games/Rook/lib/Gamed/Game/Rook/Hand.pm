package Gamed::Game::Rook::Hand;

use Moose;
extends 'Gamed::Game::Rook::Deck';

sub build_deck { [] }

no Moose;
__PACKAGE__->meta->make_immutable;

1;

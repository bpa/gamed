package Gamed::Game::Rook::State::Start;

use Moose;
with 'Gamed::Role::State';
use constant name => 'test';

no Moose;
__PACKAGE__->meta->make_immutable;

1;

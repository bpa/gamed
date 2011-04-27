class Gamed::Game::Rook::Seat;

use Gamed::Game::Rook::Hand;

has Str $.next;
has $.hand is rw;
has $.ready is rw;
has $.player is rw;
has $.passed is rw;

submethod BUILD () {
    $.hand = Gamed::Game::Rook::Hand.new;
	$.ready = True;
	$.passed = False;
	nextsame;
}

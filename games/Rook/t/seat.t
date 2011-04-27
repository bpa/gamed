use Test;
use Gamed::Game::Rook::Seat;

my $seat = Gamed::Game::Rook::Seat.new(:next<east>);
is($seat.next, 'east', "Next seat set correctly");
is($seat.ready, True, "Ready defaults to true");
is($seat.passed, False, "Passed defaults to false");
is(+$seat.hand, 0, "Start with empty hand");
done;

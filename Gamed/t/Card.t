use Test;
use Gamed::Game::Util::Card;

my $c = Card.new(suit=>'a', value=>14);
is($c.value, 14);
is($c.suit, 'a');

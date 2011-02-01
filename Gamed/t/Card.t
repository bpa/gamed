use Test;
use Gamed::Util::Card;

my $c = Gamed::Util::Card.new(suit=>'a', value=>14);
my $same = Gamed::Util::Card.new(suit=>'a', value=>14);
is($c.value, 14);
is($c.suit, 'a');

ok($c ~~ $same, "smartmatch of two equivalent succeeds");
done;

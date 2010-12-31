use Test;
use Gamed::Util::Deck;

my $d = Gamed::Util::Deck.new;
is(+$d, 0, "Deck starts empty");

my @hearts;
for 1 .. 13 { @hearts.push(Gamed::Util::Card.new(suit=>'♥', value=>$_)) }

$d.add(@hearts[0]);
 is(+$d, 1, "Have one card after adding one");
 ok($d.has_card(@hearts[0]), "Can check for card with same instance added");
 ok($d.has_card(Gamed::Util::Card.new(suit=>'♥', value=>1)), "Can check for card with a new instance that is the 'same'");
nok($d.has_card(@hearts[1]), "Check for card not in deck");

my $c = $d.draw;
is($c, @hearts[0], "Draw with no arguments draws one card");
is(+$d, 0, "No cards after drawing the last one");

$d.add(@hearts);
is(+$d, 13, "Can add an array of cards");

my @hand = $d.draw(13);
is_deeply(@hand, @hearts, "Drawing all cards returns them in order");
is(+$d, 0, "After drawing all cards, you have none");
is(@hand.elems, 13, "Return is an array");

$d.add(@hearts);
$d.shuffle;
is(+$d, 13, "Still have all the cards after shuffling");
@hand = $d.draw(13);
nok(@hand eqv @hearts, "Cards should be in a different order after shuffling");

$d.add(@hearts);
@hand = $d.draw(*);
is(@hand.elems, 13, "Drawing whatever returns all cards");
is(+$d, 0, "Drawing whatever leaves the deck empty");
is_deeply(@hand, @hearts, "Drawing all cards via whatever returns them in order");

$d.add(@hearts);
@hand = $d.pick;
is(+$d, 12, "Picking a card pulls one at random");
is(@hand.elems, 1, "The random card is returned");

@hand.push($d.pick(2));
is(+$d, 10, "Picking 2 cards removes them from the deck");
is(@hand.elems, 3, "Picking 2 cards returns 2 cards");

@hand.push($d.pick(*));
is(+$d, 0, "Picking all cards empties the deck");
is(@hand.elems, 13, "Picking whatever will return all cards in random order");

$d.add(@hearts);
@hand = $d.draw(3);
is(@hand.elems, 3, "Drawing some cards should return the number asked");
is(+$d, 10, "Drawing some cards should leave a smaller deck, but not empty");

done_testing;

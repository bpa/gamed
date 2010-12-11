use Test;
use Gamed::Util::Card;
use Gamed::Game::Rook::Hand;

my $hand = Gamed::Game::Rook::Hand.new;
my $rook = Gamed::Util::Card.new( suit => 'rook',  value => 15 );
my $b10  = Gamed::Util::Card.new( suit => 'black', value => 10 );
my $b14  = Gamed::Util::Card.new( suit => 'black', value => 14 );
my $r14  = Gamed::Util::Card.new( suit => 'red',   value => 14 );
my $g14  = Gamed::Util::Card.new( suit => 'green', value => 14 );
$hand.add( ($rook, $b10, $b14, $r14) );
is( $hand.elems, 4 );

#         is_valid_play( lead,    trump,   card)
ok( $hand.is_valid_play( 'red',   'red',   $rook ), "Play rook when trump led" );
ok(!$hand.is_valid_play( 'green', 'green', $r14 ),  "Must play rook when trump led if you don't have others" );
ok( $hand.is_valid_play( 'green', 'red',   $rook ), "Play trump when don't have lead" );
ok(!$hand.is_valid_play( 'black', 'red',   $rook ), "Play trump when have lead" );

ok(!$hand.is_valid_play( 'green', 'green', $g14 ), "Play card you don't have" );

ok(!$hand.is_valid_play( 'red',   'green', $b14 ), "Play different card when you have lead" );
ok( $hand.is_valid_play( 'black', 'green', $b14 ), "Play card of lead" );
ok( $hand.is_valid_play( 'green', 'red',   $b14 ), "Play card when you don't have lead" );

done_testing;

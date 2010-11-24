use Test::More tests => 10;
BEGIN { use_ok("Gamed::Game::Rook") }
BEGIN { use_ok("Gamed::Game::Rook::Hand") }

my $rook = new Gamed::Game::Rook::Card( suit => 'rook',  value => 15 );
my $b10  = new Gamed::Game::Rook::Card( suit => 'black', value => 10 );
my $b14  = new Gamed::Game::Rook::Card( suit => 'black', value => 14 );
my $r14  = new Gamed::Game::Rook::Card( suit => 'red',   value => 14 );
my $g14  = new Gamed::Game::Rook::Card( suit => 'green', value => 14 );
my $hand = new Gamed::Game::Rook::Hand;
$hand->add( $rook, $b10, $b14, $r14, 'hi' );
is( @{$hand->cards}, 4 );
#           is_valid_play( lead,    trump,   hand)
ok(  $rook->is_valid_play( 'red',   'red',   $hand ), "Play rook when trump led" );
ok(  $rook->is_valid_play( 'green', 'red',   $hand ), "Play rook when don't have lead" );
ok( !$rook->is_valid_play( 'black', 'red',   $hand ), "Play rook when have lead" );

ok( ! $g14->is_valid_play( 'green', 'green', $hand ), "Play card you don't have" );

ok( ! $b14->is_valid_play( 'red',   'green', $hand ), "Play different card when you have suit led" );
ok(   $b14->is_valid_play( 'black', 'green', $hand ), "Play card of suit led" );
ok(   $b14->is_valid_play( 'green', 'red',   $hand ), "Play card when you don't have suit" );

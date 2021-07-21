use Gamed::Test::Object;

class Gamed::Test::RookTest is Gamed::Test::Object;

use Gamed::Game::Rook;
use Gamed::Test::Player;

has $.server is rw;
has $.north is rw;
has $.east is rw;
has $.south is rw;
has $.west is rw;
has $.game is rw;
has @.players is rw;

submethod BUILD () {
	my @players;
    $.north = Gamed::Test::Player.new( name => 'north', id => 1 );
    $.east  = Gamed::Test::Player.new( name => 'east',  id => 2 );
    $.south = Gamed::Test::Player.new( name => 'south', id => 3 );
    $.west  = Gamed::Test::Player.new( name => 'west',  id => 4 );
    @.players = $.north, $.east, $.south, $.west;
}

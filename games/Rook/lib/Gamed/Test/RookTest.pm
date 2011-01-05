use Gamed::Test::Object;

class Gamed::Test::RookTest is Gamed::Test::Object;

use Gamed::Test::Server;
use Gamed::Game::Rook;

has $.server is rw;
has $.north is rw;
has $.east is rw;
has $.south is rw;
has $.west is rw;
has $.game is rw;
has @.players is rw;

submethod BUILD () {
	my @players;
    $.north = Gamed::Player.new( name => 'north', id => 1 );
    $.east  = Gamed::Player.new( name => 'east',  id => 2 );
    $.south = Gamed::Player.new( name => 'south', id => 3 );
    $.west  = Gamed::Player.new( name => 'west',  id => 4 );
    @.players = $.north, $.east, $.south, $.west;
	$.server = Gamed::Test::Server.new;
}

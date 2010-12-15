class Gamed::Test::RookTest;

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
    $.north = Gamed::Client.new;
    $.east  = Gamed::Client.new;
    $.south = Gamed::Client.new;
    $.west  = Gamed::Client.new;
    @.players = $.north, $.east, $.south, $.west;
	$.server = Gamed::Test::Server.new;
}

method run_tests () {
	for self.HOW.methods(self) {
        next unless $_.isa('Method') && /^test/;
		self.*setUp;
		self.$_;
		self.*tearDown;
	}
}

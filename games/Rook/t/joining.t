use Test;
use Gamed::Test::RookTest;

class t::joining is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new;
		$.game.dealer = 'south';
		$.server.reset;
    }

	method test_init() {
		is($.game.state, 'joining', "Game starts in joining state");
		is(+$.game.players, 0, "Game starts with no players");
		is(+$.game.deck, 45, "Are all the cards in the deck");
		is(+$_<hand>, 0, "Are the player hands empty") for $.game.seats.values;
	}

	method test_game_starts_with_4 () {
		for 0 .. 3 {
			$.game.player_join($.server, @.players[$_]);
			is(+$.game.players, $_+1, "Are there {$_+1} players?");
		}
		is(+$.server.broadcast, 5, "Did we get 5 messages from the game?");
		is_deeply($.server.broadcast[0], { 'player_joined' => "north" }, "Did we get a join message for north?" );
		is_deeply($.server.broadcast[1], { 'player_joined' => "east"  }, "Did we get a join message for east?" );
		is_deeply($.server.broadcast[2], { 'player_joined' => "south" }, "Did we get a join message for south?" );
		is_deeply($.server.broadcast[3], { 'player_joined' => "west"  }, "Did we get a join message for west?" );
		is_deeply($.server.broadcast[4], { 'state' => 'bidding' }, "Did we get a message to change state?");
		is($.game.state, 'bidding', "State changed to bidding");
	}

	method test_rejoin () {
		$.game.player_join($.server, $.north);
		$.game.player_join($.server, $.west);
		$.game.player_join($.server, $.south);
		$.game.player_quit($.server, $.west);
		$.game.player_join($.server, $.east);
		is(+$.game.players, 3, "There are 3 players");
		is($.east.game<seat>, 'east', "The east seat was reassigned");
	}

	method test_start_without_4 () {
		$.game.player_join($.server, $.north);
		$.game.player_join($.server, $.east);
		$.game.player_join($.server, $.south);
		$.server.reset;
		self.ready($.north);
		self.ready($.east);
		self.ready($.south, 2);
		is(+$.game.players, 3, "There are 3 players");
		is(+$.server.broadcast, 2, "Did we get 2 messages from the game on last ready?");
		is_deeply($.server.broadcast[1], { 'state' => 'bidding' }, "Did we get a message to change state?");
		is($.game.state, 'bidding', "Now bidding");
	}

	method test_ready () {
		$.game.player_join($.server, $.north);
		$.game.player_join($.server, $.east);
		$.game.player_join($.server, $.south);
		self.ready($.north);
		self.ready($.east);
		self.not-ready($.north);
		self.ready($.south);
		self.not-ready($.east);
		self.ready($.north);
		self.not-ready($.south);
		self.ready($.east);
		self.ready($.south, 2);
		is($.game.state, 'bidding', "Now bidding");
	}

	method ready ($player, $expected=1) {
		$.server.reset;
		my $count = +$.server.broadcast;
		$.game.handle_message($.server, $player, { action => 'ready' });
		is(+$.server.broadcast, $expected, "Ready was broadcast");
		is_deeply($.server.broadcast[$count], { action => 'ready', seat => $player.game<seat> }, "Ready message is right");
		is($.game.seats{$player.game<seat>}<ready>, True, "Game marked player as ready");
	}

	method not-ready ($player, $expected=1) {
		$.server.reset;
		my $count = +$.server.broadcast;
		$.game.handle_message($.server, $player, { action => 'not ready' });
		is(+$.server.broadcast, $expected, "Not ready was broadcast");
		is_deeply($.server.broadcast[$count], { action => 'not ready', seat => $player.game<seat> }, "Not ready message is right");
		is($.game.seats{$player.game<seat>}<ready>, False, "Game marked player as not ready");
	}
}

t::joining.new.run_tests;
done_testing;

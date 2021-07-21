use Test;
use Gamed::Test::Game;
use Gamed::Test::RookTest;

class t::joining is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new does Gamed::Test::Game;
		$.game.dealer = 'south';
		$.game.reset;
    }

	method test_init() {
		is($.game.state, 'joining', "Game starts in joining state");
		is(+$.game.players, 0, "Game starts with no players");
		is(+$.game.deck, 45, "Are all the cards in the deck");
		is(+$_<hand>, 0, "Are the player hands empty") for $.game.seats.values;
	}

	method test_game_starts_with_4 () {
		for 0 .. 3 {
			$.game.player_join(@.players[$_]);
			is(+$.game.players, $_+1, "Are there {$_+1} players?");
		}
		is(+$.game.broadcast, 5, "Did we get 5 messages from the game?");
		is_deeply($.game.broadcast[0], { 'player_joined' => "north" }, "Did we get a join message for north?" );
		is_deeply($.game.broadcast[1], { 'player_joined' => "east"  }, "Did we get a join message for east?" );
		is_deeply($.game.broadcast[2], { 'player_joined' => "south" }, "Did we get a join message for south?" );
		is_deeply($.game.broadcast[3], { 'player_joined' => "west"  }, "Did we get a join message for west?" );
		is_deeply($.game.broadcast[4], { 'state' => 'bidding' }, "Did we get a message to change state?");
		is($.game.state, 'bidding', "State changed to bidding");
	}

	method test_rejoin () {
		$.game.player_join($.north);
		$.game.player_join($.west);
		$.game.player_join($.south);
		$.game.player_quit($.west);
		$.game.player_join($.east);
		is(+$.game.players, 3, "There are 3 players");
		is($.east.game<seat>, 'east', "The east seat was reassigned");
		is($.game.seats<east><player>.defined, True, "The east player was added properly after rejoin");
	}

	method test_start_without_4 () {
		$.game.player_join($.north);
		$.game.player_join($.east);
		$.game.player_join($.south);
		$.game.reset;
		self.ready($.north);
		self.ready($.east);
		self.ready($.south, 2);
		is(+$.game.players, 3, "There are 3 players");
		is(+$.game.broadcast, 2, "Did we get 2 messages from the game on last ready?");
		is_deeply($.game.broadcast[1], { 'state' => 'bidding' }, "Did we get a message to change state?");
		is($.game.state, 'bidding', "Now bidding");
	}

	method test_ready () {
		$.game.player_join($.north);
		$.game.player_join($.east);
		$.game.player_join($.south);
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
		$.game.reset;
		my $count = +$.game.broadcast;
		$.game.handle_message($player, { action => 'ready' });
		is(+$.game.broadcast, $expected, "Ready was broadcast");
		is_deeply($.game.broadcast[$count], { action => 'ready', seat => $player.game<seat> }, "Ready message is right");
		is($.game.seats{$player.game<seat>}<ready>, True, "Game marked player as ready");
	}

	method not-ready ($player, $expected=1) {
		$.game.reset;
		my $count = +$.game.broadcast;
		$.game.handle_message($player, { action => 'not ready' });
		is(+$.game.broadcast, $expected, "Not ready was broadcast");
		is_deeply($.game.broadcast[$count], { action => 'not ready', seat => $player.game<seat> }, "Not ready message is right");
		is($.game.seats{$player.game<seat>}<ready>, False, "Game marked player as not ready");
	}
}

t::joining.new.run_tests;
done;

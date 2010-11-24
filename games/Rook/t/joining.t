use Test;
use Gamed::MockServer;

my $server = MockServer.new;
my $game = Rook.new;
isa_ok($game.state, Rook::State::Waiting);

/**
 * 4 players join and the games starts automatically
 */
my @players;
for 1 .. 4 → $p_id {
	my $player = Gamed::Player(name="Player $p_id");
	@players.push(Gamed::Player());
	$game.player_join($player, $server);
	is($game.playing, $p_id);
}
	
is($server.broadcast.elems, 5);

for 1 .. 4 → $id {
	is_deeply($server.broadcast[$id], { c => 'join', name => "Player $id", id => $id-1);
}

is_deeply($server.broadcast[4], { c=> 'Change State', state => 'Bidding' });
isa_ok($game.state, Rook::State::Bidding);

=begin END
/**
 * Have one player start a single player game
 * The game should move into bidding
 */
TEST_F(RookJoiningTest, start_single_player) {
	//Player *p;
	//cmd = { ROOK_CMD_READY, 0, 0, 0 };
	//player_join(game, p);
	//player_event(game, p, &cmd);
}
	void simple_command_all_test(Player *player, int command, int exp) {
		reset_mocks();
		cmd.command = command;
		player_event(game, player, (char*)&cmd, 4);
		ASSERT_EQ(exp, all_res->command);
	}

	void simple_command_plr_test(Player *player, int command, int exp) {
		reset_mocks();
		cmd.command = command;
		player_event(game, player, (char*)&cmd, 4);
		ASSERT_EQ(exp, plr_res->command);
	}


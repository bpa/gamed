use Test;

use Gamed::Client;
use Gamed::Game::HiLo;
use Gamed::Test::Server;

my $game = Gamed::Game::HiLo.new;
my $client = Gamed::Client.new;
my $server = Gamed::Test::Server.new;

sub guess(Int $before, Int $after, Int $num, Str $expected) {
    is($game.guesses, $before, "$before: Before");
	$game.handle_message($server, $client, { guess => $num });
    my @res = $server.client_msg{$client};
    is(+@res, 1, "$before: result has one element");
	@res.perl.say;
    is(@res[0]<guesses>, $before, "$before: guesses is right");
    is(@res[0]<result>, $expected, "$before: result matches");
    is($game.guesses, $after, "$before: Guess count is right in object");
    $server.reset;
}

ok($game.accepting_players, "Game starts out accepting players");
$game.player_joined($server, $client);
nok($game.accepting_players, "After joining, game isn't accepting players");
$game.number = 12;
is($game.name, 'HiLo', "Game is named");
is($game.version, '0.1', "Game has a version");

guess(0, 1, 0, 'TO_LOW');
guess(1, 2, 11, 'TOO_LOW');
guess(2, 3, 150, 'TOO_HIGH');
guess(3, 4, 13, 'TOO_HIGH');
#$server.set_random(53);
guess(4, 0, 12, 'CORRECT');
#is(53,$game.number);

#TEST_F(HiLoTest, init) {
#	destroy_instance(game);
#    set_random(100);
#    game = create_instance(&HiLo);
#    data = (HiLoData*)game->data;
#    EXPECT_EQ(1,data->number);
#    EXPECT_EQ(0,data->guesses);
#    EXPECT_EQ(0,game->playing);
#    destroy_instance(game);
#    set_random(99);
#    game = create_instance(&HiLo);
#    data = (HiLoData*)game->data;
#    EXPECT_EQ(100,data->number);
#    EXPECT_EQ(0,data->guesses);
#    EXPECT_EQ(0,game->playing);
#}
#
#TEST_F(HiLoTest, join_quit) {
#	game->accepting_players = 1;
#	player_join(game, &p);
#	EXPECT_EQ(0, game->accepting_players);
#    player_quit(game, &p);
#    EXPECT_EQ(true,game_quit);
#    create_instance(&HiLo);
#}


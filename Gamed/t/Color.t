use Test;

use Gamed::Player;
use Gamed::Game::Color;
use Gamed::Test::Server;

my $game = Gamed::Game::Color.new;
my $player = Gamed::Player.new;
my $server = Gamed::Test::Server.new;

ok($game.in_progress, "Game starts out in progress");
ok($game.accepting_players, "Game starts out accepting players");
$game.player_joined($server, $player);
nok($game.accepting_players, "After joining, game isn't accepting players");
is($game.name, 'Color', "Game is named");
is($game.version, '0.1', "Game has a version");
is($game.state, 'blue', "Game starts with start state");

$game.handle_message($server, $player, { color => 'blue' });
my $msg = $server.player_msg{$player};
is(+$msg, 1, "Got 1 message back");
is($msg[0]<color>, 'blue', "In blue state");

$server.reset;
$game.handle_message($server, $player, { color => 'red' });
$msg = $server.player_msg{$player};
is(+$msg, 1, "Got 1 player message back");
is($msg[0]<color>, 'blue', "In blue state");

my $broadcast = $server.broadcast;
is(+$broadcast, 2, "Got 2 server messages back");
is_deeply($broadcast[0], { leave => 'blue' }, "Left blue state");
is_deeply($broadcast[1], { enter => 'red' }, "Entered red state");

$server.reset;
$game.handle_message($server, $player, { color => 'red' });
$msg = $server.player_msg{$player};
is(+$msg, 1, "Got 1 player message back");
is($msg[0]<color>, 'red', "In red state");

$game.player_quit($server, $player);
nok($game.in_progress, "Game is over after the player leaves");

done_testing;

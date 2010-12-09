use Test;

use Gamed::Client;
use Gamed::Game::Color;
use Gamed::Test::Server;

my $game = Gamed::Game::Color.new;
my $client = Gamed::Client.new;
my $server = Gamed::Test::Server.new;

ok($game.in_progress, "Game starts out in progress");
ok($game.accepting_players, "Game starts out accepting players");
$game.player_joined($server, $client);
nok($game.accepting_players, "After joining, game isn't accepting players");
is($game.name, 'Color', "Game is named");
is($game.version, '0.1', "Game has a version");
is($game.state, 'blue', "Game starts with start state");

$game.handle_message($server, $client, { color => 'blue' });
my $msg = $server.client_msg{$client};
is(+$msg, 1, "Got 1 message back");
is($msg[0]<color>, 'blue', "In blue state");

$server.reset;
$game.handle_message($server, $client, { color => 'red' });
$msg = $server.client_msg{$client};
is(+$msg, 1, "Got 1 client message back");
is($msg[0]<color>, 'blue', "In blue state");

my $broadcast = $server.broadcast;
is(+$broadcast, 2, "Got 2 server messages back");
is_deeply($broadcast[0], ( leave => 'blue' ), "Left blue state");
is_deeply($broadcast[1], ( enter => 'red' ), "Entered red state");

$server.reset;
$game.handle_message($server, $client, { color => 'red' });
$msg = $server.client_msg{$client};
is(+$msg, 1, "Got 1 client message back");
is($msg[0]<color>, 'red', "In red state");

$game.player_quit($server, $client);
nok($game.in_progress, "Game is over after the player leaves");

done_testing;

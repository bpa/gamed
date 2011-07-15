use Test;

use Gamed::Game::Color;
use Gamed::Test::Game;
use Gamed::Test::Player;

my $game = Gamed::Game::Color.new() does Gamed::Test::Game;
my $player = Gamed::Test::Player.new;

ok($game.in_progress, "Game starts out in progress");
ok($game.accepting_players, "Game starts out accepting players");
$game.player_joined($player);
nok($game.accepting_players, "After joining, game isn't accepting players");
is($game.name, 'Color', "Game is named");
is($game.version, '0.1', "Game has a version");
is($game.state, 'blue', "Game starts with start state");

$game.handle_message($player, { color => 'blue' });
my $msg = $player.messages;
is(+$msg, 1, "Got 1 message back");
is($msg[0]<color>, 'blue', "In blue state");

$game.reset;
$game.handle_message($player, { color => 'red' });
$msg = $player.messages;
is(+$msg, 1, "Got 1 player message back");
is($msg[0]<color>, 'blue', "In blue state");

my $broadcast = $game.broadcast;
is(+$broadcast, 2, "Got 2 server messages back");
is_deeply($broadcast[0], { leave => 'blue' }, "Left blue state");
is_deeply($broadcast[1], { enter => 'red' }, "Entered red state");

$game.reset;
$game.handle_message($player, { color => 'red' });
$msg = $player.messages;
is(+$msg, 1, "Got 1 player message back");
is($msg[0]<color>, 'red', "In red state");

$game.player_quit($player);
nok($game.in_progress, "Game is over after the player leaves");

done;

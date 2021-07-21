use Test;

use Gamed::Game::HiLo;
use Gamed::Test::Game;
use Gamed::Test::Player;

my $game = Gamed::Game::HiLo.new but Gamed::Test::Game;
my $player = Gamed::Test::Player.new;

sub guess(Int $before, Int $after, Int $num, Str $expected) {
    is($game.guesses, $before, "$before: Before");
	$game.handle_message($player, { guess => $num });
    my $res = $player.messages;
    is(+$res, 1, "$before: result has one element");
    is($res[0]<guesses>, $before+1, "$before: guesses is right");
    is($res[0]<result>, $expected, "$before: result matches");
    is($game.guesses, $after, "$before: Guess count is right in object");
    $game.reset;
}

ok($game.in_progress, "Game starts out in progress");
ok($game.accepting_players, "Game starts out accepting players");
$game.player_join($player);
nok($game.accepting_players, "After joining, game isn't accepting players");
$game.number = 12;
is($game.name, 'HiLo', "Game is named");
is($game.version, '0.1', "Game has a version");

guess(0, 1, 0, 'Too low');
guess(1, 2, 11, 'Too low');
guess(2, 3, 150, 'Too high');
guess(3, 4, 13, 'Too high');
guess(4, 0, 12, 'Correct');

$game.number = 150;
guess(0, 0, 150, 'Correct');
ok($game.number <= 100, "The new number is <= 100");
ok($game.number > 0, "The new number is > 0");

$game.player_quit($player);
nok($game.in_progress, "Game is over after the player leaves");

done;

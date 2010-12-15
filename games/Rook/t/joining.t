use Test;
use Gamed::Game::Rook;
use Gamed::Test::Server;
use Gamed::Client;

my $server = Gamed::Test::Server.new;
my $game = Gamed::Game::Rook.new;
is($game.state, 'joining', "Game starts in joining state");

# 4 players join and the games starts automatically
my @players;
for 1 .. 4 -> $p_id {
	my $player = Gamed::Client.new;#(name="Player $p_id");
	@players.push($player);
	$game.player_join($server, $player);
	is(+$game.players, $p_id, "Are there $p_id players?");
}
	
is(+$server.broadcast, 5, "Did we get 5 messages from the game?");
is_deeply($server.broadcast[0], ( 'player_joined' => "north" ), "Did we get a join message for north?" );
is_deeply($server.broadcast[1], ( 'player_joined' => "east"  ), "Did we get a join message for east?" );
is_deeply($server.broadcast[2], ( 'player_joined' => "south" ), "Did we get a join message for south?" );
is_deeply($server.broadcast[3], ( 'player_joined' => "west"  ), "Did we get a join message for west?" );
is_deeply($server.broadcast[4], ( 'state' => 'bidding' ), "Did we get a message to change state?");
is($game.state, 'bidding', "State changed to bidding");

done_testing;

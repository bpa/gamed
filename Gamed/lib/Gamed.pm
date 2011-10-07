#use Gamed::Server::Commands;

class Gamed;# does Gamed::Server::Commands;
use Gamed::Player;
use Gamed::Game::HiLo;
use Gamed::Game::None;

has %!games;
has %!players;

method handle($type, $player, %msg={}) {
	given $type {
		when 1 {
			%!players{$player.id} = $player;
			$player.game = Gamed::Game::None.new;
		}
		when 0 {
			given %msg<s> {
				when 'join' { $player.send({ s=>'error', m=>'Game not found', name=>%msg<name> }) }
				when 'create' {
					%!games{%msg<name>} = $player.game = Gamed::Game::HiLo.new;
					$player.send({ s=>'create', game=>%msg<game>, name=>%msg<name> });
					$player.game.player_join($player);
				}
				when 'quit' {
					$player.game.player_quit($player);
					$player.game = Gamed::Game::None.new;
				}
				default { $player.game.handle_message($player, %msg) }
			}
		}
		when -1 {
			%!players{$player.id}.delete;
			$player.game.player_quit($player);
		}
	}
}

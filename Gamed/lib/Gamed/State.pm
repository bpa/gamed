role Gamed::State;

use Gamed::Player;
use Gamed::Game;

has Str $.name is rw;

method enter_state ( Gamed::Game $game ) { }
multi method handle_message ( Gamed::Game $game, Gamed::Player $player, %msg ) { ... }
method leave_state ( Gamed::Game $game ) { }

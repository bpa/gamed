role Gamed::State;

has Str $.name is rw;

method enter_state ( $game ) { }
multi method handle_message ( $game, $player, %msg ) { ... }
method leave_state ( $game ) { }

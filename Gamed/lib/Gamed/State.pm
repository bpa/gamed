class Gamed::State;

has Str $.name is rw;

method enter_state ( Gamed::Server $server, Gamed::Game $game ) { ... }
multi method handle_message ( Gamed::Server $server, Gamed::Game $game, Gamed::Client $client, %msg ) { ... }
method leave_state ( Gamed::Server $server, Gamed::Game $game ) { ... }

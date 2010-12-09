role Gamed::Role::StateMachine;

use Gamed::Client;
use Gamed::State;

has Str $!_current_state;
has Gamed::State %!states;
has Gamed::State $!_state;

multi method state () {
	return $!_current_state;
}

multi method change_state ( Str $state, Gamed::Server $server? ) {
	$!_state.leave_state($server, self) if $!_state;
	$!_state = %!states{$state};
	$!_current_state = $state;
	$!_state.enter_state($server, self);
}

multi method handle_message( Gamed::Server $server, Gamed::Client $client, %msg ) {
	$!_state.handle_message( $server, self, $client, %msg );
}

method add_states ( *@states ) {
    for @states {
        %!states{$_.name} = $_;
    }
}

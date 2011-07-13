role Gamed::Role::StateMachine;

use Gamed::Player;
use Gamed::State;

has Str $!_current_state;
has Gamed::State %!states;
has Gamed::State $!_state;

multi method state () {
	return $!_current_state;
}

multi method change_state ( Str $state ) {
	die "No state '$state' exists" unless %!states.exists($state);
	$!_state.leave_state(self) if $!_state;
	$!_state = %!states{$state};
	$!_current_state = $state;
	$!_state.enter_state(self);
}

multi method handle_message( Gamed::Player $player, %msg ) {
	$!_state.handle_message( self, $player, %msg );
}

method add_states ( *@states ) {
    for @states {
        %!states{$_.name} = $_;
    }
}

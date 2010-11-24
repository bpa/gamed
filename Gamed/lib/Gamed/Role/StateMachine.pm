role Gamed::Role::StateMachine {
    use Gamed::Role::Client;
    #use Gamed::Role::Message;
    use Gamed::Role::State;
    use Gamed::Role::Pluggable;
    does Gamed::Role::Pluggable;

    has Str $.start_state = 'default';
    has Str $!state;
    has Gamed::Role::State %!states;
    has Gamed::Role::State $!_state;

    multi method state () {
        return $!state;
    }

    multi method state ( Str $state ) {
        die "No state $state defined" unless defined %!states{$state};
        $!_state.leave(self) if $!_state;
        $!_state = %!states{$state};
        $!state = $state;
        $!_state.enter(self);
    }

    submethod BUILD () {
        %!states = self.plugins( 'Gamed::Role::State', self.WHAT ~ '::State' );
        die "No starting state [{$.start_state}] exists for {self.WHAT}" unless %!states{$.start_state};
        self.state($.start_state);
    }

    #submethod handle ( Gamed::Role::Client $client, Gamed::Role::Message $message ) {
    submethod handle ( Gamed::Role::Client $client, $message ) {
        say "StateMachine delegating to {$!state}";
        $!_state.handle( $client, $message);
    }
}

1;

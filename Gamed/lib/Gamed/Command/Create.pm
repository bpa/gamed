class Gamed::Command::Create {
    use Gamed::Role::Command;
    does Gamed::Role::Command;
    has Str $.name = 'create';

    method handle ( $gamed, $client, $message ) {
        my ( $cmd, $game, $name ) = $message;
        if ( $gamed.games.exists($game) ) {
            my $instance = $gamed.instances{$name};
            if ( !$instance) {
                $instance = eval($gamed.games{$game}.WHAT).new;
                $gamed.instances{$name} = $instance;
            }
            $instance.add($client);
            $instance.announce( [ <game join>, { player => $client.name, id => $client.player_id } ] );
        }
        else {
            $client.send( [ 'game', 'error', 'Game not available' ] );
        }
    }

}

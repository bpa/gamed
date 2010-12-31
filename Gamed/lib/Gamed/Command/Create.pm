class Gamed::Command::Create {
    use Gamed::Role::Command;
    does Gamed::Role::Command;
    has Str $.name = 'create';

    method handle ( $gamed, $player, $message ) {
        my ( $cmd, $game, $name ) = $message;
        if ( $gamed.games.exists($game) ) {
            my $instance = $gamed.instances{$name};
            if ( !$instance) {
                $instance = eval($gamed.games{$game}.WHAT).new;
                $gamed.instances{$name} = $instance;
            }
            $instance.add($player);
            $instance.announce( [ <game join>, { player => $player.name, id => $player.player_id } ] );
        }
        else {
            $player.send( [ 'game', 'error', 'Game not available' ] );
        }
    }

}

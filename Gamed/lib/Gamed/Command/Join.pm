class Gamed::Command::Join {
    use Gamed::Role::Command;
    does Gamed::Role::Command;

    has Str $.name = 'join';

    method handle ( $gamed, $player, $message ) {
        my ( $cmd, $name ) = $message;
        my $instance = $gamed.instances{$name};
        if ( $instance ) {
            if ( $instance.is_join_allowed($player) ) {
                $instance.add($player);
                $instance.announce( [ game => 'join' => { player => $player.name, id => $player.player_id } ] );
            }
            else {
                $player.send( [ game => error => $player.error ] );
            }
        }
        else {
            $player.send( [ game => error => 'Game not available' ] );
        }
    }

}

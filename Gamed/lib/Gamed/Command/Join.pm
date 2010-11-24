class Gamed::Command::Join {
    use Gamed::Role::Command;
    does Gamed::Role::Command;

    has Str $.name = 'join';

    method handle ( $gamed, $client, $message ) {
        my ( $cmd, $name ) = $message;
        my $instance = $gamed.instances{$name};
        if ( $instance ) {
            if ( $instance.is_join_allowed($client) ) {
                $instance.add($client);
                $instance.announce( [ game => 'join' => { player => $client.name, id => $client.player_id } ] );
            }
            else {
                $client.send( [ game => error => $client.error ] );
            }
        }
        else {
            $client.send( [ game => error => 'Game not available' ] );
        }
    }

}

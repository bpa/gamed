class Gamed::Game::Color::State::Red {
    use Gamed::Role::State;
    does Gamed::Role::State;

    has Str $.name = 'red';

    method handle ( $client, $message ) {
        $client.send( { color => 'red' } );
        if ( $message{'color'} eq 'blue' ) {
            $client.game.state('blue');
        }
    }

    method leave ( $game ) {
        $game.announce( { leave => 'red' } );
    }

}

class Gamed::Game::Color::State::Blue {
    use Gamed::Role::State;
    does Gamed::Role::State;

    has $.name = 'blue';

    method enter ( $game ) {
        $game.announce( { enter => 'blue' } );
    }

    method handle ( $client, $message ) {
        say "Red handling message {$message.perl}";
        $client.send( { color => 'blue' } );
        if ( $message{'color'} eq 'red' ) {
            $client.game.state('red');
        }
    }

    method leave ( $game ) {
        $game.announce( { leave => 'blue' } );
    }

}

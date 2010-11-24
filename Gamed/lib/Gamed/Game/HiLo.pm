class Gamed::Game::HiLo {
    use Gamed::Role::Game;
    does Gamed::Role::Game;

    has Str $.name    = 'HiLo';
    has Num $.version = '0.1';
    has Int $.number  = int( 100.rand ) + 1;
    has Int $!guesses = 0;

    method handle ( $client, $message ) {
        say $message.perl;
        my %resp;
        %resp<guesses> = ++$!guesses;
        if ( $message<guess> == self.number ) {
            %resp<result> = 'correct';
            $.number = int( 100.rand ) + 1;
        }
        else {
            %resp<result> =
                 $message<guess> < $.number
              ?? 'too low'
              !! 'too high';
        }
        $client.send( \%resp );
    }

}

use Gamed::Game;

class Gamed::Game::HiLo is Gamed::Game;

has Int $.number is rw;
has Int $.guesses is rw;

submethod BUILD {
    $.name    = 'HiLo';
    $.version = '0.1';
}

method player_joined( $server, $player ) {
    $.number  = 100.rand.ceiling;
    $.guesses = 0;
    $.accepting_players = False;
}

method handle_message ( Gamed::Server $server, Gamed::Player $player, %msg ) {
    $.guesses++;
    my %resp = { guesses => $.guesses };
    if ( %msg<guess> == $.number ) {
        %resp<result> = 'Correct';
        $.number = 100.rand.ceiling;
        $.guesses = 0;
    }
    else {
        %resp<result> =
             %msg<guess> < $.number
          ?? 'Too low'
          !! 'Too high';
    }
    $server.send( %resp, $player );
}

method player_quit (Gamed::Server $server, Gamed::Player $player) {
    $.in_progress = False;
    $.accepting_players = True;
}

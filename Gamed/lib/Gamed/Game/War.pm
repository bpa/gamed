class Gamed::Game::War {
    use Gamed::Role::Game;
    does Gamed::Role::Game;

    has Str $.name        = 'War';
    has Str $.version     = '0.1';
    has Int @!deck;

    submethod BUILD() {
        $.max_players = 2;
    }

    method build_deck {
            for 1 .. 3 {
                for 1 .. 9 -> $val {
                    push @!deck, $val;
                }
            }
        }

    method handle ( $game, $client, $message ) {
    }

}

role Gamed::Role::Client {
    requires &.send;

    use Gamed::Role::Game;
    has Gamed::Role::Game $.game;
    has Str $.name is rw;
    has Int $.player_id is rw;

    submethod BUILD () {
        $.name = 'nobody';
        $.player_id = 0;
    }

    method in_game () { defined $.game }
}

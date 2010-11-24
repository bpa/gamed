class Gamed::Game::Color {
    use Gamed::Role::Game;
    use Gamed::Role::StateMachine;
    does Gamed::Role::Game;
    does Gamed::Role::StateMachine;

    has Str $.name        = 'color';
    has Str $.version     = '0.1';

    submethod BUILD {
        $.start_state = 'blue';
    }
}

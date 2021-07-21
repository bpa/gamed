use Gamed::Game;

class Gamed::Game::None is Gamed::Game;

multi method handle_message (Gamed::Player $player, %msg) { }
method player_join (Gamed::Player $player) { }
method player_quit (Gamed::Player $player) { }

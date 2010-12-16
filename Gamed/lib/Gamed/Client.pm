class Gamed::Client;

has %.game is rw;
has $.game_instance is rw;
has Str $.name is rw;

method in_game () { return $.game_instance.defined }

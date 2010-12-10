class Gamed::Client;

has $.game;
has Str $.name;

method in_game () { return $.game.defined }

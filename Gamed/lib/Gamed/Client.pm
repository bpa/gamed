class Gamed::Client;

has $.game;
has Str $.name;
has Int $.player_id;

method in_game () { return $.game.defined }

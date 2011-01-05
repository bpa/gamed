class Gamed::Player;

has %.game is rw;
has Str $.name;
has Int $.id;

multi method Str() {
	return $.id;
}

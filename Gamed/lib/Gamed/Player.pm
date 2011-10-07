class Gamed::Player;

has $!client;
has $.game is rw;
has $.id;

method send(%msg) {
	$!client.send(%msg);
}

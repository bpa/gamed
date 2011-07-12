class Gamed::Player;

use JSON::Tiny;

has %.game is rw;
has Str $.name;
has Int $.id;
has $!sock;

multi method Str() {
	return $.id;
}

method send(%msg) {
	$!sock.send(to-json(%msg));
}

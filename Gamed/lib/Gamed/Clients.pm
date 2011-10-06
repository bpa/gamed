class Gamed::Clients;

use NativeCall;
use JSON::Tiny;

sub listen(Int $port) returns Boolean is native('client');

sub next_event() is export {
}

method recv() {
	say $!sock;
	my $r = $!sock.recv;
	say $r;
	say "got it";
	return from-json($r);
	#return from-json($!sock.recv);
}

method send(%msg) {
	$!sock.send(to-json(%msg));
}

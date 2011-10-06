class Gamed::Client;

use NativeCall;
use JSON::Tiny;

method listen ($sock) {
	my $websocket = WebSocket.new(:sock($sock));
	return self.bless(*, :sock($websocket));
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

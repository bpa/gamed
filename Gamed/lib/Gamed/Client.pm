class Gamed::Client;

use WebSocket;
use JSON::Tiny;
has $!sock;

method new ($sock) {
	my $websocket = WebSocket.wrap($sock);
	$websocket.do-server-handshake;
	return self.bless(*, :sock($websocket));
}

method recv() {
	return from-json($!sock.recv);
}

method send(%msg) {
	$!sock.send(to-json(%msg));
}

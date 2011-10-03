class Gamed::Player;

has $!game;
has $!client;

method send(%msg) {
	$!client.send(%msg);
}

method recv() {
	$!client.recv();
}

class Gamed::Player;

has $!client;
has $game is rw;

method send(%msg) {
	$!client.send(%msg);
}

method recv() {
	$!client.recv();
}

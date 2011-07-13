use Gamed::Player;

class Gamed::Test::Player is Gamed::Player;

has @.messages is rw;

submethod send ( %msg ) {
	my $msg = %msg;
	@.messages.push($msg);
}

method reset() {
	@.messages = ();
}

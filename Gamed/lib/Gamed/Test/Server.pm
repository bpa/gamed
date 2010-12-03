class Gamed::Test::Server is Gamed::Server;

has %.client_msg is rw;
has @.broadcast is rw;

multi method send ( %msg ) {
    @.broadcast.push(%msg);
}

multi method send ( %msg, $client ) {
	%.client_msg{$client} = [] unless %.client_msg{$client}.defined;
    %.client_msg{$client}.push(%msg);
}

method reset () {
    %.client_msg = {};
    @.broadcast = ();
}

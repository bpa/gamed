role Gamed::Test::Game;

has @.broadcast is rw;

multi method send ( %msg ) {
    my $msg = %msg;
    @.broadcast.push($msg);
}

multi method send ( %msg, $player ) {
    $player.send(%msg);
}

method reset () {
    @.broadcast = ();
	%!players.values>>.reset;
}

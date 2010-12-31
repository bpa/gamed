use Gamed::Server;

class Gamed::Test::Server is Gamed::Server;

has %.player_msg is rw;
has @.broadcast is rw;

multi method send ( %msg ) {
    my $msg = %msg;
    @.broadcast.push($msg);
}

multi method send ( %msg, $player ) {
    my $msg = %msg;
	%.player_msg{$player} = Array.new unless %.player_msg{$player}.defined;
    %.player_msg{$player}.push($msg);
}

method reset () {
    %.player_msg = ();
    @.broadcast = ();
}

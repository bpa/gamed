#use Gamed::Server::Commands;

class Gamed;# does Gamed::Server::Commands;

has %!games;
has @!players;

method handle_message($player, %msg) {
	say %msg;
}

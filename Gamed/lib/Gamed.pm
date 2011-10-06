#use Gamed::Server::Commands;

class Gamed;# does Gamed::Server::Commands;

has %!games;
has @!players;

method handle($type?, $msg?, $client?) {
	say "$type $msg";
}

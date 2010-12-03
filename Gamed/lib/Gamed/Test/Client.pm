use Gamed::Client;

class Gamed::Test::Client is Gamed::Client;

has @!messages;

method send ( %msg ) {
    @!messages.push(%msg);
}

method next () {
    return @!messages.shift;
}

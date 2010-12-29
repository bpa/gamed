use Gamed::Player;

class Gamed::Test::Client is Gamed::Player;

has @!messages;

method send ( %msg ) {
    @!messages.push(%msg);
}

method next () {
    return @!messages.shift;
}

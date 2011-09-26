use Gamed::Client;

class Gamed::Bot is Gamed::Client;
use Gamed::Personality;

has Gamed::Personality $.personality;

method join_game( $server ) { ... }
method handle_message( $server, %msg ) { ... }
method quit_game( $server ) { ... }

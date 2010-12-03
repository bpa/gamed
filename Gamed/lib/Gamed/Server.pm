class Gamed::Server;

use Gamed::Client;

has Gamed::Client @.clients;

multi method send ( %msg ) { ... }

multi method send ( %msg, $client ) { ... }


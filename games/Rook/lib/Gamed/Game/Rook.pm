use Gamed::Game;
use Gamed::Role::StateMachine;

class Gamed::Game::Rook is Gamed::Game does Gamed::Role::StateMachine;

use Gamed::Game::Rook::PartnershipDeck;
use Gamed::Game::Rook::Joining;
use Gamed::Game::Rook::Bidding;

has %.seats is rw;
has $.deck is rw;
has %.players is rw;

submethod BUILD {
	$.name    = 'Rook';
	$.version = '0.1';
    $.deck = Gamed::Game::Rook::PartnershipDeck.new;
    for <north east south west> { %.seats{$_} = { hand => Gamed::Util::Deck.new } };

	self.add_states(
		Gamed::Game::Rook::Joining.new,
		Gamed::Game::Rook::Bidding.new);
    self.change_state('joining');
}

method player_join (Gamed::Server $server, Gamed::Client $client) {
    for %.seats.pairs {
        if !$_.value<player>.defined {
            $_.value<player> = $client;
            %.players{$client} = $_.key;
            last;
        }
    }
    
    $server.send( { player_joined => %.players{$client} } );
    self.change_state('bidding', $server) if +%.players == 4;
}

method player_quit (Gamed::Server $server, Gamed::Client $client) {
    %.seats{%.players{$client}}.delete('player');
    %.players.delete($client);
}

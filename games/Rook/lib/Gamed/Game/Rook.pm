use Gamed::Game;
use Gamed::Role::StateMachine;

class Gamed::Game::Rook is Gamed::Game does Gamed::Role::StateMachine;

use Gamed::Game::Rook::PartnershipDeck;
use Gamed::Game::Rook::Joining;
use Gamed::Game::Rook::Bidding;

has %.seats is rw;
has $.deck is rw;
has $.players is rw;
has Int $.bid is rw;
has Str $.bidder is rw;
has Str $.dealer is rw;
has Str $.current_player is rw;
has Int $.passed is rw;
has @!seats;

submethod BUILD {
    @!seats = <north east south west>;
	$.name    = 'Rook';
	$.version = '0.1';
    $.deck = Gamed::Game::Rook::PartnershipDeck.new;
    %.seats<north> = { hand => Gamed::Util::Deck.new, next => 'east'  };
    %.seats<east>  = { hand => Gamed::Util::Deck.new, next => 'south' };
    %.seats<south> = { hand => Gamed::Util::Deck.new, next => 'west'  };
    %.seats<west>  = { hand => Gamed::Util::Deck.new, next => 'north' };
    $.players = 0;
    $.dealer = 'north';
    $.current_player = 'north';

	self.add_states(
		Gamed::Game::Rook::Joining.new,
		Gamed::Game::Rook::Bidding.new);
    self.change_state('joining');
}

method player_join (Gamed::Server $server, Gamed::Client $client) {
	$.in_progress = True;

	# For now, maintain ordering for tests
    for @!seats -> $dir {
		my $seat = %.seats{$dir};
        if !$seat<player>.defined {
            $seat<player> = $client;
            $client.game<seat> = $dir;
            $.players++;
            last;
        }
    }
    
    $server.send( { player_joined => $client.game<seat> } );
    self.change_state('bidding', $server) if $.players == 4;
}

method player_quit (Gamed::Server $server, Gamed::Client $client) {
    $.players--;
    %.seats{$client.game<seat>}<player> = Any;
	$.in_progress = False if $.players < 1;
}

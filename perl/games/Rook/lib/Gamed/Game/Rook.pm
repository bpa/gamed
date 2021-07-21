use Gamed::Game;
use Gamed::Role::StateMachine;

class Gamed::Game::Rook is Gamed::Game does Gamed::Role::StateMachine;

use Gamed::Game::Rook::PartnershipDeck;
use Gamed::Game::Rook::Joining;
use Gamed::Game::Rook::Bidding;
use Gamed::Game::Rook::Picking;
use Gamed::Game::Rook::Playing;

#TODO: move bidder, bid, trump into round
has %.seats is rw;
has $.deck is rw;
has $.players is rw;
has Int $.bid is rw;
has Str $.trump is rw;
has Str $.bidder is rw;
has Str $.dealer is rw;
has Str $.current_player is rw;
has Int $.passed is rw;
has %.round is rw;
has @!seats;

submethod BUILD {
    @!seats = <north east south west>;
	$.name    = 'Rook';
	$.version = '0.1';
    $.deck = Gamed::Game::Rook::PartnershipDeck.new;
    %.seats<north> = { hand => Gamed::Util::Deck.new, ready => True, next => 'east'  };
    %.seats<east>  = { hand => Gamed::Util::Deck.new, ready => True, next => 'south' };
    %.seats<south> = { hand => Gamed::Util::Deck.new, ready => True, next => 'west'  };
    %.seats<west>  = { hand => Gamed::Util::Deck.new, ready => True, next => 'north' };
    $.players = 0;
    $.dealer = 'north';
    $.current_player = 'north';

	self.add_states(
		Gamed::Game::Rook::Joining.new,
		Gamed::Game::Rook::Bidding.new,
		Gamed::Game::Rook::Picking.new,
		Gamed::Game::Rook::Playing.new);
    self.change_state('joining');
}

method player_join (Gamed::Player $player) {
	$.in_progress = True;

	# For now, maintain ordering for tests
    for @!seats -> $dir {
		my $seat = %.seats{$dir};
        if !$seat<player>.defined {
            $seat<player> = $player;
            $seat<ready> = False;
            $player.game = { seat => $dir };
            $.players++;
			%!players{$dir} = $player;
            last;
        }
    }
    
    self.send( { player_joined => $player.game<seat> } );
    self.change_state('bidding') if $.players == 4;
}

method player_quit (Gamed::Player $player) {
    $.players--;
    %.seats{$player.game<seat>}<player> = Mu;
	%!players.delete($player.game<seat>);
	$.in_progress = False if $.players < 1;
}

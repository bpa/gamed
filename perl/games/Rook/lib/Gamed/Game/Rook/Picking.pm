use Gamed::State;

class Gamed::Game::Rook::Picking is Gamed::State;

submethod BUILD () {
	$.name = 'picking';
}

method enter_state ( Gamed::Game $game ) {
	$game.send( { 'state' => 'picking' } );
	$game.send( { action => 'nest', cards => $game.deck }, $game.seats{$game.bidder}<player> );
	$game.seats{$game.bidder}<hand>.add($game.deck.pick(*));
}

multi method handle_message ( Gamed::Game $game, Gamed::Player $player, %msg ) {
#    unsigned char mask, trump;
#    unsigned char card;
#    RookData *rook = (RookData*)game->data;
#    RookPlayer *rp = &rook->players[p->in_game_id];
#    if (p->in_game_id != rook->bidder) {
#        give_error(p, ROOK_ERR_NOT_YOUR_TURN);
#    }
#    else if (cmd->command == ROOK_CMD_CHOOSE_TRUMP) {
#        if (rook->trump) {
#            give_error(p, ROOK_ERR_TRUMP_CHOSEN);
#            return;
#        }
#        trump = cmd->card & 0xF0;
#        mask = 0x10;
#        for (i=0; i<4; i++) {
#            if (trump & mask) {
#                mask_count++;
#            }
#            mask <<= 1;
#        } 
#        if (mask_count == 1) {
#            rook->trump = trump;
#            all_cmd_pc(game, ROOK_CMD_CHOOSE_TRUMP, rook->bidder, trump);
#        }
#        else {
#            give_error(p, ROOK_ERR_INVALID_TRUMP);
#        }
#    }
#    else if (cmd->command == ROOK_CMD_CHOOSE_WIDOW) {
#        for (i=1; i<6; i++) {
#            card = ((unsigned char*)cmd)[i];
#            if (!has_card(rp->hand, card)) {
#                give_error(p, ROOK_ERR_DONT_HOLD_CARD);
#            }
#        }
#    }
}

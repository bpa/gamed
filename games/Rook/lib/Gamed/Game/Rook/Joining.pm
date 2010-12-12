use Gamed::State;

class Gamed::Game::Rook::Joining is Gamed::State;

submethod BUILD () {
    $.name = 'joining';
}

method enter_state ( Gamed::Server $server, Gamed::Game $game ) {
    $server.send( { 'state' => 'joining' } );
#    int i;
#    Deck *hand;
#    hand = (Deck*)rook->deck;
#    shuffle(hand);
#    hand = (Deck*)rook->discard;
#    hand->cards = 0;
#    hand = (Deck*)rook->nest;
#    hand->cards = 0;
#    for (i=0; i<4; i++) {
#        rook->players[i].pass = false;
#        hand = (Deck*)&rook->players[i].hand;
#        hand->cards = 0;
#    }
#    rook->bid = 95;
#    rook->bidder = rook->current_player;
#    rook->passed = 0;
#    rook->trump = '\0';
#
}

=begin END
multi method handle_message (Gamed::Server $server, Gamed::Client $client, %msg) {
#    RookData *rook = (RookData*)g->data;
#    if (p->in_game_id != rook->current_player) {
#        give_error(p, ROOK_ERR_NOT_YOUR_TURN);
#    }
#    else if (cmd->command == ROOK_CMD_PASS) {
#        rook->players[p->in_game_id].pass = true;
#        all_cmd_p(game, ROOK_CMD_PASS, p->in_game_id);
#        rook->passed++;
#        bid_next_player;
#        if (rook->passed == 3) {
#            rook->bidder = rook->current_player;
#            rook->state = ROOK_PICKING_TRUMP;
#            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
#        }
#    }
#    else if (cmd->score <= rook->bid) {
#        give_error(p, ROOK_ERR_BID_TOO_LOW);
#    }
#    else if (cmd->score > 200) {
#        give_error(p, ROOK_ERR_BID_TOO_HIGH);
#    }
#    else if (cmd->score % 5 != 0) {
#        give_error(p, ROOK_ERR_INVALID_BID);
#    }
#    else {
#        rook->bid = cmd->score;
#        rook->bidder = p->in_game_id;
#        all_cmd_ps(game, ROOK_CMD_BID, p->in_game_id, rook->bid);
#        if (rook->bid == 200) {
#            rook->state = ROOK_PICKING_TRUMP;
#            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
#            return;
#        }
#        bid_next_player;
#    }
}


use Gamed::State;

class Gamed::Game::Rook::Bidding is Gamed::State;

submethod BUILD () { 
    $.name = 'bidding';
}

method enter_state ( Gamed::Server $server, Gamed::Game $rook ) {
    $rook.accepting_players = False;
    $server.send( { 'state' => 'bidding' } );
    $rook.bid = 95;
    $rook.bidder = $rook.current_player;
    $rook.passed = 0;
}

method handle_message ( Gamed::Server $server, Gamed::Game $rook, Gamed::Client $client, %msg ) {
    if ($client.name !~~ $rook.current_player) {
        $server.send( { error => 'Not your turn' }, $client );
        return;
    }

    given %msg<bid> {
        when 'pass' {
#        rook->players[p->in_game_id].pass = true;
#        all_cmd_p(game, ROOK_CMD_PASS, p->in_game_id);
#        rook->passed++;
#        bid_next_player;
#        if (rook->passed == 3) {
#            rook->bidder = rook->current_player;
#            rook->state = ROOK_PICKING_TRUMP;
#            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
        }
        when $_ <= $rook.bid {
            $server.send( { error => 'bid too low' }, $client );
        }
        when $_ > 200 {
            $server.send( { error => 'bid too high' }, $client );
        }
        when $_ % 5 != 0 {
            $server.send( { error => 'bid invalid' }, $client );
        }
        default {
#        rook->bid = cmd->score;
#        rook->bidder = p->in_game_id;
#        all_cmd_ps(game, ROOK_CMD_BID, p->in_game_id, rook->bid);
#        if (rook->bid == 200) {
#            rook->state = ROOK_PICKING_TRUMP;
#            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
#            return;
#        }
#        bid_next_player;
        }
    }
}


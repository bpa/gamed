use Test;
use Gamed::Test::RookTest;

class t::bidding is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new;
    	$.game.player_join($.server, $_) for @.players;
		$.game.bid = 150;
		$.game.bidder = 'south';
    	$.game.change_state('picking');
		$.game.trump = 'red';
		$.game.deck.add($.game.seats<south><hand>.draw(5));
		$.server.reset;
    	$.game.deck = Gamed::Game::Rook::PartnershipDeck.new;
		$.game.change_state('playing', $.server);
    }
    	
    method test_init () {
		is($.game.current_player, 'south', 'High bidder starts');
		is(+$.server.broadcast, 1, "State change message broadcast");
		is_deeply($.server.broadcast[0], { 'state' => 'playing' }, "Broadcast state message is correct");
    	is($.game.state, 'playing', 'Now playing');
		is($.game.round<ns_points>, 0, 'ns start with 0 points');
		is($.game.round<ew_points>, 0, 'ew start with 0 points');
		is(+$.game.round<tricks>, 0, 'taken tricks is empty');
    }
    
}

t::bidding.new.run_tests;
done;

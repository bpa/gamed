use Test;
use Gamed::Test::RookTest;

class t::bidding is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new;
    	$.game.player_join($.server, $_) for @.players;
    	$.game.change_state('bidding');
    	$.server.reset;
    }
    	
    method test_init () {
    	is($.game.state, 'bidding', 'Now bidding');
    	is($.game.bid, 95, 'Starting off, the bid is 95');
    	is($.game.bidder, 'north', "Bid winner is the first player to start");
        is($.game.passed, 0, 'No one has passed');
    }
    
    method test_normal_bid () {
        self.pass($.north);
        self.pass($.east);
        self.bid($.south, 120);
        self.bid($.west,  140);
        self.bid($.south, 150);
        self.pass($.west);
    
    	is($.game.bid, 150, "Winning bid is 150");
        is($.game.state, 'picking', 'Now picking trump');
    	is($.game.bidder, 'south', "South took the bid");
        is($.game.current_player, 'south', "South leads");
    }
    
    method test_max_bid() {
        self.bid($.north, 200);

    	is($.game.bid, 200, "Winning bid is 200");
        is($.game.state, 'picking', 'Now picking trump');
    	is($.game.bidder, 'north', "North took the bid");
        is($.game.current_player, 'north', "North leads");
    }

    method test_all_pass() {
        self.pass($.north);
        self.pass($.east);
        self.pass($.south);

    	is($.game.bid, 100, "Winning bid is 100 (forced open)");
        is($.game.state, 'picking', 'Now picking trump');
    	is($.game.bidder, 'west', "West took the bid by default");
        is($.game.current_player, 'west', "West leads");
    }

    method test_invalid_bids() {
        self.invalid_bid($.north,  95, 'bid too low');
        self.invalid_bid($.north,   0, 'bid too low');
    
        self.invalid_bid($.north, 205, 'bid too high');
        self.invalid_bid($.north, Inf, 'bid too high');
    
        self.invalid_bid($.west,  25, 'not your turn');
        self.invalid_bid($.east, 150, 'not your turn');
        self.invalid_bid($.south, 155, 'not your turn');
    
        self.invalid_bid($.north, 101, 'bid invalid');
        self.invalid_bid($.north, 104, 'bid invalid');
        self.invalid_bid($.north, 199, 'bid invalid');
    
        self.pass($.north);
        self.bid($.east, 105);
        self.bid($.south, 110);
        self.bid($.west, 115);
        is('east', $.game.current_player, "Skip north in rotation after pass");
        self.invalid_bid($.north, 120, 'not your turn');
        self.invalid_bid($.east, 105, 'bid too low');
    }
    
    method pass ( $player ) {
        $.game.handle_message( $.server, $player, { bid => 'pass' } );
    }
    
    method bid ( $player, $bid ) {
        $.game.handle_message( $.server, $player, { bid => $bid } );
    }
    
    method invalid_bid ( $player, $bid, $error ) {
        $.game.handle_message( $.server, $player, { bid => $bid } );
    }

}

t::bidding.new.run_tests;
done_testing;

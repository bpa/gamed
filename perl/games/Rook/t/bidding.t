use Test;
use Gamed::Test::RookTest;
use Gamed::Test::Game;

class t::bidding is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new does Gamed::Test::Game;
    	$.game.player_join($_) for @.players;
		$.game.dealer = 'south';
    	$.game.change_state('bidding');
    	$.game.reset;
    }
    	
    method test_init () {
		$.game.bid = 150;
		$.game.bidder = 'south';
		$.game.dealer = 'south';
		$.game.passed = 4;
		$.game.current_player = 'south';
		for $.game.seats.values {
			$_<passed> = True;
		}

		$.game.change_state('bidding');
		is(+$.game.broadcast, 1, "State change message broadcast");
		is_deeply($.game.broadcast[0], { 'state' => 'bidding' }, "Broadcast state message is correct");
    	is($.game.state, 'bidding', 'Now bidding');
    	is($.game.bid, 95, 'Starting off, the bid is 95');
    	is($.game.bidder, Any, "No one has bid yet");
		is($.game.dealer, 'west', "Dealer changes");
    	is($.game.current_player, 'north', "player right of dealer is first");
        is($.game.passed, 0, 'No one has passed');

		for @.players {
			is($_.messages[0]<action>, 'deal', "{$_.name} was dealt to");
			is(+$_.messages[0]<cards>, 10, "{$_.name} was dealt 10 cards");
		}

		for $.game.seats.pairs {
			nok($_.value<passed>, "{$_.key} pass flag is clear");
			is(+$_.value<hand>, 10, "{$_.key} has 10 cards");
		}

		is(+$.game.deck, 5, "The nest holds 5 cards");
    }
    
    method test_normal_bid () {
        self.pass($.north);
        self.pass($.east);
        self.bid($.south, 120);
        self.bid($.west,  140);
        self.bid($.south, 150);
        self.pass($.west);

		is(+$.game.broadcast, 1, "Got state change message");
		is(+$.south.messages, 1, "South got message");
		is($.south.messages[0]<action>, 'nest', "South sent the nest");

    	is($.game.bid, 150, "Winning bid is 150");
        is($.game.state, 'picking', 'Now picking trump');
    	is($.game.bidder, 'south', "South took the bid");
        is($.game.current_player, 'south', "South leads");
    }
    
    method test_max_bid() {
        self.bid($.north, 200, 1);
		is(+$.game.broadcast, 1, "Got state change message");
		is(+$.north.messages, 1, "North got message");
		is($.north.messages[0]<action>, 'nest', "North sent the nest");

    	is($.game.bid, 200, "Winning bid is 200");
        is($.game.state, 'picking', 'Now picking trump');
    	is($.game.bidder, 'north', "North took the bid");
        is($.game.current_player, 'north', "North leads");
    }

    method test_all_pass() {
        self.pass($.north);
        self.pass($.east);
        self.pass($.south);

		is(+$.game.broadcast, 1, "Got state change message");
		is(+$.west.messages, 1, "West got message");
		is($.west.messages[0]<action>, 'nest', "West sent the nest");

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
    
	method test_next_player () {
		is($.game.current_player, 'north', "Starting with north");
		$.game.seats<north><passed> = True;
		$.game.seats<east><passed> = True;
		my $bidding = Gamed::Game::Rook::Bidding.new;
		$bidding.next_player($.game);
		is($.game.current_player, 'south', "skip east");
		$bidding.next_player($.game);
		is($.game.current_player, 'west', "west didn't pass");
		$bidding.next_player($.game);
		is($.game.current_player, 'south', "north and east passed");
	}

    method pass ( $player ) {
		$.game.reset;
        $.game.handle_message( $player, { bid => 'pass' } );
		is(+$player.messages, 0, "Pass was broadcast");
		ok(+$.game.broadcast > 0, "Everyone got pass message");
		my $pass = $.game.broadcast.shift;
		is_deeply($pass, { player => $player.game<seat>, event => 'bid', bid => 'pass' } );
    }
    
    method bid ( $player, $bid, $got_bid=0 ) {
		$.game.reset;
        $.game.handle_message( $player, { bid => $bid } );
		is(+$player.messages, $got_bid, "Bid was broadcast");
		ok(+$.game.broadcast > 0, "Everyone got bid message");
		my $msg = $.game.broadcast.shift;
		is_deeply($msg, { player => $player.game<seat>, event => 'bid', bid => $bid } );
    }
    
    method invalid_bid ( $player, $bid, $error ) {
		$.game.reset;
        $.game.handle_message( $player, { bid => $bid } );
		is(+$player.messages, 1, "Bid error was sent to player");
		is(+$.game.broadcast, 0, "Error was not broadcast");
		is_deeply($player.messages[0], { event => 'error', msg => $error } );
		$.game.reset;
    }

}

t::bidding.new.run_tests;
done;

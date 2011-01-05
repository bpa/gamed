use Test;
use Gamed::Test::RookTest;

class t::picking is Gamed::Test::RookTest {

    method setUp () {
    	$.game = Gamed::Game::Rook.new;
    	$.game.player_join($.server, $_) for @.players;
		$.game.bid = 150;
		$.game.bidder = 'south';
		$.game.current_player = 'south';
    	$.server.reset;
    	$.game.change_state('picking');
    }

	method test_init() {
		is(+$.server.player_msg, 1, "Widow was sent");
		is(+$.server.player_msg{$.south}, 1, "To south");
		#TODO check cards sent
		is(+$.game.seats<north><hand>, 10, "North has 10 cards");
		is(+$.game.seats<east><hand>,  10, "East has 10 cards");
		is(+$.game.seats<south><hand>, 15, "South has 15 cards");
		is(+$.game.seats<west><hand>,  10, "West has 10 cards");
	}

#unsigned char dealt_cards[] = { 0xF0, 0x81, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A,
#                                            0x49, 0x47, 0x45, 0x28, 0x26,
#                                      0x1C, 0x1A, 0x19, 0x17, 0x16, 0x15 };

	method test_valid () {
#	    valid_trump(pS, 0,0,0,1);
#	    cmd.card = 0x20;
#	    handle_request(&game, &pS, (char*)&cmd, 4); \
#	    ASSERT_EQ(ROOK_CMD_ERROR, plr_res->command); \
#	    ASSERT_EQ(ROOK_ERR_TRUMP_CHOSEN, err->error);
	}
	
	method test_invalid_picks() {
#	    // Not real cards
#	    char bad1[] = {ROOK_CMD_CHOOSE_WIDOW, 0xFF, 0x82, 0x83, 0x84, 0x71 };
#	    invalid_pick(pS, bad1);
#	
#	    // Last one isn't a real card
#	    char bad2[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x25 };
#	    invalid_pick(pS, bad2);
#	
#	    // Last one isn't a real card
#	    char bad3[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x00 };
#	    invalid_pick(pS, bad3);
#	
#	    // Can't give two of the same
#	    char bad4[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x28 };
#	    invalid_pick(pS, bad4);
#	
#	    // Can't give two of the same
#	    char bad5[] = {ROOK_CMD_CHOOSE_WIDOW, 0xF0, 0x47, 0x45, 0x28, 0xF0 };
#	    invalid_pick(pS, bad5);
	}
}

t::picking.new.run_tests;
done_testing;


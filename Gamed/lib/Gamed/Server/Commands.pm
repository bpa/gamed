role Gamed::Server::Commands;

use Plugins;
has %!available_games;

submethod BUILD () {
	for load_plugins('Gamed::Game') {
		%!available_games{$_} = 1;
	}
}

method handle_message( $player, $msg ) {
	my $cmd = $msg{s};
	return False unless $cmd.defined;
	return unless .^can("cmd_$cmd");
	self."cmd_$cmd";
	return True;
}

method cmd_create ( $player, $msg ) {
	if ( %!games.exists($game) ) {
		my $instance = $gamed.instances{$name};
		if ( !$instance) {
			$instance = eval($gamed.games{$game}.WHAT).new;
			$gamed.instances{$name} = $instance;
		}
		$instance.add($player);
		$instance.announce( [ <game join>, { player => $player.name, id => $player.player_id } ] );
	}
	else {
		$player.send( [ 'game', 'error', 'Game not available' ] );
	}
}

method cmd_join ( $gamed, $player, $message ) {
	my ( $cmd, $name ) = $message;
	my $instance = $gamed.instances{$name};
	if ( $instance ) {
		if ( $instance.is_join_allowed($player) ) {
			$instance.add($player);
			$instance.announce( [ game => 'join' => { player => $player.name, id => $player.player_id } ] );
		}
		else {
			$player.send( [ game => error => $player.error ] );
		}
	}
	else {
		$player.send( [ game => error => 'Game not available' ] );
	}
}

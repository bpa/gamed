module Game;

sub games_available() is export {
	my @games;
	for dir('Game', :test(/.*\.p<[m6]>$/)) -> $file {
		my $module = "Game/$file";
		$module ~~ s:g/\//::/;
		$module ~~ s/...$//;
		@games.push($module);
	}
	return @games;
}

sub client(Str $game) is export {
	require $game;
	return $game.new;
}

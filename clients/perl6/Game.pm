module Game;

sub games_available() is export {
	my @games;
	for dir('Game', :test(/.*\.p<[m6]>$/)) -> $file {
		next if $file ~~ 'Base.pm';
		my $module = $file;
		$module ~~ s:g/\//::/;
		$module ~~ s/...$//;
		@games.push($module);
	}
	return @games;
}

sub client(Str $game) is export {
	my $module = "Game::$game";
	require $module;
	return "$module.new".eval;
}

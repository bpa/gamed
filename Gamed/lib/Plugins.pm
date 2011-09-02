module Plugins;

sub load_plugins(Str $module?) is export {
    my @plugins;
	for @*INC -> $inc {
		for dir("$inc/$dir", :$test) -> $file {
			next if $file ~~ @exclude;
			my $module = $file;
			$module ~~ s:g/\//::/;
			$module ~~ s/...$//;
			try {
				need $module;
				@plugins.push($module);
			}
		}
	}
	return @plugins;
}

role Commands;

method handle_command(Str $cmd) {
		my &f = %commands{$cmd};
		if (&f.defined) {
			try {
				&f(:$cmd, |@args);
				CATCH {
					say $!;
					say gather {
						take $cmd;
						for &f.signature.params -> $s {
							take $s.name.substr(1) unless $s.optional;
						}
					}.join(' ');
				}
			}
		}
}

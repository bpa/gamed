class Gamed::Test::Object;

use Test;

method run_tests () {
	for self.^methods {
        next unless $_.isa('Method') && /^test/;
		ok(True, "**** Running $_ ****");
		self.*setUp;
		self.$_;
		self.*tearDown;
	}
}

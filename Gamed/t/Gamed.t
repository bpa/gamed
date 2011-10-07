use Test;
use Gamed;
use Gamed::Test::Player;

my $g = Gamed.new;
my $p = Gamed::Test::Player.new;

$g.handle(1, $p);
isa_ok($p.game, Gamed::Game::None);

$g.handle(0, $p, { s=>'join', name=>'test' });
is(+$p.messages, 1);
my %msg = $p.messages.pop;
is_deeply(%msg, { s=>'error', m=>'Game not found', name=> 'test' });

$g.handle(0, $p, { s=>'create', game=>'HiLo', name=>'test' });
is(+$p.messages, 1);
%msg = $p.messages.pop;
is_deeply(%msg, { s=>'create', game=>'HiLo', name=> 'test' });

isa_ok($p.game, Gamed::Game::HiLo);

$g.handle(0, $p, { guess => 0 } );
is(+$p.messages, 1);
%msg = $p.messages.pop;
is_deeply(%msg, { guesses=>1, result=>'Too low' });

$g.handle(0, $p, { s=>'quit' });
isa_ok($p.game, Gamed::Game::None);

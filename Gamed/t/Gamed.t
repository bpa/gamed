use Test;
use Gamed;

my $g = Gamed.new;
my $p = Gamed::Test::Player.new;

$g.player_join($p);

$g.handle_message($p, { s=>'join', game=>'HiLo' });
is(+$p.messages, 1);
my %msg = $p.messages.pop;
is_deeply(%msg, { s=>'error', m=>'Game not found', game=> 'HiLo' });

#$g.handle_message($p, { s=>'create', game=>'HiLo', name=>'test' });

use Gamed::Util::Card;

class Gamed::Game::Rook::Card is Gamed::Util::Card;

#submethod BUILD (Str $str) {
#	say "$str => {self}";
#	if $str.lc eq 'r15' {
#		return Gamed::Game::Rook::Card.new(:suit<rook>, :value(15));
#	}
#	else {
#		my $suit;
#		given $str.comb(/<alpha>/).lc {
#		  when 'b' { $suit = 'black' }
#		  when 'g' { $suit = 'green' }
#		  when 'r' { $suit = 'red' }
#		  default  { $suit = 'yellow' }
#		}
#		return Gamed::Game::Rook::Card.new(:$suit, :value(int($str.comb(/\d+/))));
#	}
#}

method Str() {
	return 'ROOK' if $.suit ~~ 'rook';
	return $.suit.substr(0,1).upper ~ $.value;
}

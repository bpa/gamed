class WebSocket;

use WebSocket::v0;
use WebSocket::v8;

method new ($sock) {
	my $header = $!sock.recv;
	my %h;
	my $bits;
	for $header.split("\r\n") -> $l {
		%h.push($l.split(': ')) if $l.index(': ');
		$bits = $l if $l;
	}
	given %h<Sec-WebSocket-Version> {
		when 0 .. 7 { return WebSocket::v0.new($sock, %h, $bits); }
		default     { return WebSocket::v8.new($sock, %h, $bits); }
	}
}

class WebSocket;

use Digest::MD5;
has $!sock;

method wrap($sock) {
	return self.bless(*, :sock($sock));
}

multi method do-server-handshake() {
	my $header = $!sock.recv;
say $header;
	my %h;
	my $bits;
	for $header.split("\n") -> $l {
		%h.push($l.split(': ')) if $l.index(': ');
		$bits = $l if $l;
	}
	self.do-server-handshake(%h, $bits, %h<Sec-WebSocket-Version>);
}

multi method do-server-handshake(%header, $extra, $version where ($version < 8)) {
	my @key;
	for 1,2 -> $x {
		my $a = %header{"Sec-WebSocket-Key$x"};
		my $v = $a.comb(/<digit>/).join / +$a.comb(/\ /);
		@key.push: pack('N', $v).unpack('A*');
	}
	@key.push($extra);
	my $r = Digest::MD5.md5_hex(@key);
	$r = $r.comb(/<xdigit>**2/).map: { chr '0x' ~ $_ };
	$!sock.send("HTTP/1.1 101 WebSocket Protocol Handshake
Upgrade: WebSocket
Connection: Upgrade
Sec-WebSocket-Origin: %header<Origin>
Sec-WebSocket-Location: ws://localhost:3939/
Sec-WebSocket-Protocol: sample

{$r.join}");
}

multi method do-server-handshake(%header, $extra, $version) {
	my @key;
	for 1,2 -> $x {
		my $a = %header{"Sec-WebSocket-Key$x"};
		my $v = $a.comb(/<digit>/).join / +$a.comb(/\ /);
		@key.push: pack('N', $v).unpack('A*');
	}
	@key.push($extra);
	my $r = Digest::MD5.md5_hex(@key);
	$r = $r.comb(/<xdigit>**2/).map: { chr '0x' ~ $_ };
	$!sock.send("HTTP/1.1 101 Switching Protocols
Upgrade: WebSocket
Connection: Upgrade
Sec-WebSocket-Origin: %header<Origin>
Sec-WebSocket-Location: ws://localhost:3939/
Sec-WebSocket-Protocol: chat

}

method send(Str $str) {
	say "Sending: $str";
	my $buf = $str.encode;
	$buf.contents.unshift(0);
	$buf.contents.push(255);
	$!sock.send($buf.unpack("A*"));
}

method recv() {
	my $buf = $!sock.recv.encode;
	$buf.contents.pop;
	$buf.contents.shift;
	say "Received: {$buf.decode}";
	return $buf.decode;
}

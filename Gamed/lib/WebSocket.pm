class WebSocket {

use Digest::MD5;
use Support;
has $!sock is rw;

multi method do-server-handshake() {
	my $header = $!sock.recv;
	my %h;
	my $bits;
	for $header.split("\r\n") -> $l {
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
	$!sock.send("HTTP/1.1 101 WebSocket Protocol Handshake\r
Upgrade: WebSocket\r
Connection: Upgrade\r
Sec-WebSocket-Origin: %header<Origin>\r
Sec-WebSocket-Location: ws://localhost:3939/\r
Sec-WebSocket-Protocol: sample\r
\r
{$r.join}");
}

multi method do-server-handshake(%header, $extra, $version) {
	my $k = %header<Sec-WebSocket-Key>;
	$k ~= '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
	my $ret = sha1_base64($k);
	$!sock.send("HTTP/1.1 101 Switching Protocols\r
Upgrade: WebSocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: $ret\r
Sec-WebSocket-Location: ws://localhost:3939/\r
Sec-WebSocket-Origin: %header<Sec-WebSocket-Origin>\r\n\r\n");
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

}

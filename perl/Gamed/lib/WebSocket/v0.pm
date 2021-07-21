class WebSocket::v0;

use Digest::MD5;
has $!sock;

method new($sock, %header, $extra) {
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
	return self.bless(*, :sock($sock));
}

method send(Str $str) {
	my $buf = $str.encode;
	$buf.contents.unshift(0);
	$buf.contents.push(255);
	$!sock.send($buf.unpack("A*"));
}

method recv() {
	my $buf = $!sock.recv.encode;
	$buf.contents.pop;
	$buf.contents.shift;
	return $buf.decode;
}

class WebSocket::v8;

use Support;
has $!sock;

method new($sock, %header, $extra) {
	my $k = %header<Sec-WebSocket-Key>;
	$k ~= '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';
	my $ret = sha1_base64($k);
	$!sock.send("HTTP/1.1 101 Switching Protocols\r
Upgrade: WebSocket\r
Connection: Upgrade\r
Sec-WebSocket-Accept: $ret\r
Sec-WebSocket-Location: ws://localhost:3939/\r
Sec-WebSocket-Origin: %header<Sec-WebSocket-Origin>\r\n\r\n");
	return self.bless(*, :sock($sock));
}

method send(Str $str) {
	say "Sending: $str";
	my $buf = $str.encode;
	$buf.contents.unshift(0);
	$buf.contents.push(255);
	$!sock.send($buf.unpack("A*"));
}

method recv() {
	say "hello";
	my $buf = $!sock.recv;
	say $buf.perl;
	$buf = $buf.encode;
	#$buf.contents.pop;
	#$buf.contents.shift;
	say "Received: {$buf.decode}";
	return $buf.decode;
}

use Test;
use WebSocket;

class SockMock {
	has Str $in is rw;
	has Str $out is rw;
	method send ($data) { $out ~= $data; }
	method recv () { my $buf = $in; $in = ''; return $buf; }
	method out() { $out }
}

my $v8 = 
"GET / HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Host: localhost:3939
Sec-WebSocket-Origin: null
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 8
";

my $v8_conn = SockMock.new(:in($v8));
my $ws = WebSocket.new(:sock($v8_conn));
$ws.do-server-handshake;
my %h = parse-header($v8_conn.out);
is %h<Sec-WebSocket-Accept>, "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", "Handshake server key";

sub parse-header(Str $data) {
	my %h;
	for $data.split("\r\n") -> $l {
		%h.push($l.split(': ')) if $l.index(': ');
	}
	return %h;
}

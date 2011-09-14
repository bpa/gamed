class WebSocket;

use Digest::MD5;
has IO::Socket $!sock;

method new (*%args) {
	my $s = IO::Socket::INET.new(|%args))
	but role {
		
	}
	self.bless(*, :sock(IO::Socket::INET.new(|%args)))
}

method ready () { 
	fail("Not connected") unless $!sock.PI0; 
	return $!sock.PI0.poll(1, 0, 0); 
}

method accept() {
	my $c = $!sock.accept;
	my $header = $c.recv;
	my %h;
	my $bits;
	for $header.split("\n") -> $l {
		%h.push($l.split(': ')) if $l.index(': ');
		$bits = $l if $l;
	}
	my @key;
	for 1,2 -> $x {
		my $a = %h{"Sec-WebSocket-Key$x"};
		my $v = $a.comb(/<digit>/).join / +$a.comb(/\ /);
		@key.push: pack('N', $v).unpack('A*');
	}
	@key.push($bits);
	my $r = Digest::MD5.md5_hex(@key);
	$r = $r.comb(/<xdigit>**2/).map: { chr '0x' ~ $_ };
	$c.send("HTTP/1.1 101 WebSocket Protocol Handshake
Upgrade: WebSocket
Connection: Upgrade
Sec-WebSocket-Origin: %h<Origin>
Sec-WebSocket-Location: ws://localhost:3939/
Sec-WebSocket-Protocol: sample

{$r.join}");
	return self.bless(*, :sock($c));
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

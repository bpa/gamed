role NormalNetOpts {
	method ready () { 
		fail("Not connected") unless $!PIO; 
		return $!PIO.poll(1, 0, 0); 
	}

	method failed () { 
		fail("Not connected") unless $!PIO; 
		return $!PIO.poll(0, 0, 1); 
	}
}

class Gamed::Socket::Wrapper {
	does IO::Socket;
	does NormalNetOpts;
	method recv() {
		return $!PIO.recv;
	}
}

class Gamed::Socket is IO::Socket::INET does NormalNetOpts;

submethod accept() {
	return Gamed::Socket::Wrapper.new(:PIO($!PIO.accept));
}

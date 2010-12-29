#! /usr/local/bin/perl6

use v6;

my $PF_INET     = 2; # these should move into a file,
my $SOCK_STREAM = 1; # but what name and directory?
my $TCP         = 6;
my $server = IO::Socket::INET.socket( $PF_INET, $SOCK_STREAM, $TCP );
$server.bind( 'localhost', 3939 );
$server.listen();
say "Listening";
while my $player = $server.accept() {
    say "Accepted";
    my $received = $player.recv();
    $player.send( $received.chomp ~ " to you too!");
    $player.close();
}


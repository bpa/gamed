#! /usr/bin/perl

use strict;
use warnings;
use YAML::Any qw/LoadFile/;

open my $games, ">games.h" or die ("Can't open games.h for writing: $!\n");
print $games "#ifndef GAMES_DEFINITION\n";
print $games "#define GAMES_DEFINITION\n";
opendir(my $dir, '.');
for my $f (grep { /yml$/ } readdir $dir) {
	my $yaml = LoadFile($f);
	my ($name) = $f =~ /(.*)\.yml$/;
	open my $source, ">$name.c" or die("Can't open $name.c: $!\n");
	write_c_file($source, $name);
	print $games "Board board_init_$name();\n";
}
print $games "#endif\n";

sub write_c_file {
	my ($source, $name) = @_;
	print $source <<HEADER;
Board board_init_$name() {
	Board board = (Board) malloc(sizeof(Board));
	board->countries = $countries
	board->borders = (char *) malloc($table_size);
	}
HEADER
	print $source "\n";
}

#! /usr/bin/perl

use strict;
use warnings;
use YAML::Any qw/LoadFile/;

open my $header, ">boards.h" or die ("Can't open boards.h for writing: $!\n");
open my $source, ">boards.c" or die("Can't open boards.c for writing: $!\n");
print $header "#ifndef BOARDS_DEFINITION\n";
print $header "#define BOARDS_DEFINITION\n";
print $header "#include \"board.h\"\n";
opendir(my $dir, '.');
for my $f (grep { /yml$/ } readdir $dir) {
	my $yaml = LoadFile($f);
	my ($name) = $f =~ /(.*)\.yml$/;
	write_borders($name, $yaml);
	write_bonuses($name, $yaml);
	write_init($name, $yaml);
	print $header "Board board_init_$name();\n";
}
print $header "#endif\n";

sub write_borders {
}

sub write_bonuses {
}

sub write_init {
	my ($name, $yaml) = @_;
	my $players = $yaml->{players};
	print $source <<INIT;
Board board_init_$name() {
	Board board = (Board) malloc(sizeof(Board));
	board->players = $players;
	board->borders = &$name\_borders;
	board->bonuses = &$name\_bonuses;
	return board;
}
INIT
}

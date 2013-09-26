#! /usr/bin/perl

use strict;
use warnings;
use YAML::Any qw/LoadFile/;

open my $header, ">boards.h" or die ("Can't open boards.h for writing: $!\n");
open my $source, ">boards.c" or die("Can't open boards.c for writing: $!\n");
print $header "#ifndef BOARDS_DEFINITION\n";
print $header "#define BOARDS_DEFINITION\n";
print $source "#include \"board.h\"\n";
print $source "#include <stdlib.h>\n";
opendir(my $dir, '.');
for my $f (grep { /yml$/ } readdir $dir) {
	my $yaml = LoadFile($f);
	my ($name) = $f =~ /(.*)\.yml$/;
	$yaml->{id} = make_map($yaml);
	write_borders($name, $yaml);
	write_bonuses($name, $yaml);
	write_init($name, $yaml);
	print $header "Board *board_init_$name();\n";
}
print $header "#endif\n";

sub make_map {
	my $yaml = shift;
	my %terr;
	my $ind = 0;
	for my $t (@{$yaml->{territories}}) {
		$terr{$t->{name}} = $ind++;
	}
	return \%terr;
}

sub write_borders {
	my ($name, $yaml) = @_;
	my @table;
	for my $t (@{$yaml->{territories}}) {
		my $a = $yaml->{id}{$t->{name}};
		for my $border (@{$t->{borders}}) {
			my $b = $yaml->{id}{$border};
			next unless $b;
			$table[$a][$b] = 1;
			$table[$b][$a] = 1;
		}
	}
	my $len = scalar(@{$yaml->{territories}});
	my @lines;
	for (my $i=0; $i<$len; $i++) {
		my @line;
		for (my $j=0; $j<$len; $j++) {
			push @line, $table[$i][$j] ? 1 : 0;
		}
		push @lines, "  {" . join(',', @line) . "}";
	}
	print $source "char $name\_borders[$len][$len] = {\n";
	print $source join(",\n", @lines);
	print $source "\n};\n";
}

sub write_bonuses {
	my ($name, $yaml) = @_;
	$yaml->{regions} ||= {};
	$yaml->{continents} ||= {};
	my $regions = scalar(keys $yaml->{regions}) + scalar(keys $yaml->{continents});
	my @bonuses;
	for my $k ('regions', 'continents') {
		for my $region (values $yaml->{$k}) {
			my @territories;
			for my $subregion (@{$region->{regions}}) {
				for my $t (@{$yaml->{regions}{$subregion}{territories}}) {
					push @territories, $yaml->{id}{$t};
				}
			}
			for my $t (@{$region->{territories}}) {
				push @territories, $yaml->{id}{$t};
			}
			@territories = sort { $a <=> $b } @territories;
			print $source "char $name\_req" . scalar(@bonuses) . "[] = {" . join(',', @territories) . "};\n";
			push @bonuses, "  {" . ($region->{bonus} || 0) . ',' . scalar(@territories) . ",\&$name\_req" . scalar(@bonuses) . '[0]}';
		}
	}

	print $source "Bonus $name\_bonuses[$regions] = {\n";
	print $source join(",\n", @bonuses);
	print $source "\n};\n";
}

sub write_init {
	my ($name, $yaml) = @_;
	my $players = $yaml->{players};
	my $period = $yaml->{army_generation_period} || 25;
	my $territories = scalar(@{$yaml->{territories}});
	print $source <<INIT;
Board *board_init_$name() {
	Board *board = (Board *) malloc(sizeof(Board));
	board->max_players = $players;
	board->army_generation_period = $period;
	board->territories = $territories;
	board->borders = &$name\_borders[0][0];
	board->bonuses = &$name\_bonuses[0];
	return board;
}
INIT
}

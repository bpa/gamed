#! /usr/bin/perl

use strict;
use warnings;
use YAML::Any qw/LoadFile/;
use File::Slurp;

open my $source, ">boards.cc" or die("Can't open boards.cc for writing: $!\n");
print $source "//Generated content, do not modify.\n//Update yml files and then run: perl build_from_yaml\n";

opendir(my $dir, '.');
for my $f (grep { /yml$/ } readdir $dir) {
	my $yaml = LoadFile($f);
	my ($name) = $f =~ /(.*)\.yml$/;
	$yaml->{id} = make_map($yaml);
	write_borders($name, $yaml);
	write_bonuses($name, $yaml);
	write_init($name, $yaml);
	update_test_file($name, $yaml);

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
			unless (defined $b) {
				print "Can't find $border in $name\n";
				next;
			}
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
			print $source "int $name\_req" . scalar(@bonuses) . "[] = {" . join(',', @territories) . "};\n";
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
	my $game = $yaml->{name};
	my $version = $yaml->{version};
	my $bonuses = scalar(keys $yaml->{regions}) + scalar(keys $yaml->{continents});
	print $source "int $name\_starting_armies[] = {", join (',', @{$yaml->{starting_armies}}), "};\n";
	print $source <<INIT;
Board board_$name = {
	.max_players = $players,
	.starting_armies = &$name\_starting_armies[0],
	.army_generation_period = $period,
	.territories = $territories,
	.regions = $bonuses,
	.borders = &$name\_borders[0][0],
	.bonuses = &$name\_bonuses[0]
};
void game_init_$name(GameInstance *g, const Server *s) {
	game_init(g, s, &board_$name);
}
Game $game = { GAMED_GAME, "$game", "$version", game_init_$name, NULL, player_join, player_quit };
INIT
}

sub update_test_file {
	my ($name, $yaml) = @_;
	return unless $name eq "classic";
	my $generated = '#define SR_MAX_PLAYERS ' . $yaml->{players} . "\n";
	$generated .= "#define SR_NUM_COUNTRIES 42\n";
	my $test_file = read_file("test.h");
	for my $i (0 .. $#{$yaml->{territories}}) {
		my $territory = uc($yaml->{territories}[$i]{name});
		$territory =~ tr/ /_/;
		$generated .= "#define SR_$territory $i\n";
	}
	$test_file =~ s#(//Start generated\n).*(//end generated)#$1$generated$2#is;
	write_file("test.h", $test_file);
}

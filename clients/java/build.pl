#! /usr/bin/perl

use strict;
use warnings;
use YAML::Any qw/LoadFile/;
use File::Slurp;
use Image::Magick;

my $name = $ARGV[0];
my $dir = "../../resources/$name"."Risk";
my $board = read_file("src/gamed/client/SpeedRisk/$name"."Board.java");
my $yaml = LoadFile("$dir/board.yml");
$yaml->{id} = make_map($yaml);
my $countries = countries($name, $yaml);
my $borders = borders($name, $yaml);
$board =~ s/(private static void addCountries\(\)\s+{).*?(}\s+public)/$1$countries$borders$2/s;
write_file("src/gamed/client/SpeedRisk/$name"."Board.java", $board);

sub make_map {
	my $yaml = shift;
	my %terr;
	my $ind = 0;
	for my $t (@{$yaml->{territories}}) {
		$terr{$t->{name}} = $ind++;
	}
	return \%terr;
}

sub countries {
	my ($name, $yaml) = @_;
	my $countries = "\n";
	for my $c (0 .. $#{$yaml->{territories}}) {
		my $country = $yaml->{territories}[$c];
		my $cname = $country->{name};
		my $loc = "resources/$name"."Risk/$cname.png";
		my $img = Image::Magick->new();
		$img->Read("../../$loc");
		my $coords = $img->Get('comment');
		$countries .= "\t\tCOUNTRIES[$c] = new Country($c, \"$loc\", $coords);\n";
	}
	$countries .= "\t}\n\n\tstatic { addCountries(); }\n\n";
	return $countries;
}

sub borders {
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
	my $func = '';
	my $count = 0;
	for my $i (0 .. $#{$yaml->{territories}}) {
		if ($i % 50 == 0) {
			$func .= "\t}\n\n" if $count;
			$count++;
			$func .= "\tstatic { addBorders$count(); }\n\n";
			$func .= "\tprivate static void addBorders$count()\n\t{\n";
		}
		my @line;
		for (my $j=0; $j<$#{$yaml->{territories}}; $j++) {
			push @line, $table[$i][$j] ? "true" : "false";
		}
		$func .= "\t\tBORDERS[$i] = new boolean[] {" . join(',', @line) . "};\n";
	}
	return $func;
}


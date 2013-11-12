#! /usr/bin/perl 
use strict;
use warnings;

use Image::Magick;
use Data::Dumper;

use constant MAX => 4;

sub fix {
    my $image = shift;
    my $im = Image::Magick->new;
    $im->Read($image);
    my ( $h, $w ) = $im->Get( 'height', 'width' );

    my @alpha;
    for my $x ( 0 .. $w ) {
        for my $y ( 0 .. $h ) {
            my ($z) = $im->GetPixel( x => $x, y => $y, channel => 'RGB' );
            $alpha[ $x + $y * $w ] = $z ? 1 : 0;
        }
    }

    for my $x ( 0 .. $w ) {
        for my $y ( 0 .. $h ) {
            write_pixels( \@alpha, $x, $y, $x, $y, $w, $h )
              unless $alpha[ $x + $y * $w ];
        }
    }
    #write_pixels( \@alpha, 32, 1, 32, 1, $w, $h );

    my $copy = Image::Magick->new( size => $w . 'x' . $h );
    $copy->Read('NULL:black');

    for my $x ( 0 .. $w ) {
        for my $y ( 0 .. $h ) {
            my $a = $alpha[ $x + $y * $w ];
            $copy->SetPixel(
                x       => $x,
                y       => $y,
                channel => 'all',
                color   => $a ? [ 1, 1, 1, 1 - $a ] : 'black' );
        }
    }

	$copy->Quantize(
		#colors              => 2,
		colorspace          => 'Gray',
		'transparent-color' => 'black',
	);
    my $x = $copy->Write($image);
    warn $x if $x;
}

sub write_pixels {
    my ( $im, $sx, $sy, $x, $y, $w, $h ) = @_;

    set_pixel( $im, $sx, $sy, $x + 1, $y,     $w, $h );
    set_pixel( $im, $sx, $sy, $x,     $y + 1, $w, $h );
    set_pixel( $im, $sx, $sy, $x - 1, $y,     $w, $h );
    set_pixel( $im, $sx, $sy, $x,     $y - 1, $w, $h );
}

sub set_pixel {
    my ( $im, $sx, $sy, $x, $y, $w, $h ) = @_;

    return if $x < 0 || $y < 0 || $x >= $w || $y >= $h;

    my $dist = sqrt( abs( $sx - $x )**2 + abs( $sy - $y )**2 ) / MAX;
    return unless 0 < $dist && $dist < 1;

    my ($a) = $im->[ $x + $y * $w ];
    my $d = $a - $dist;
    #print "Set: $sx, $sy, $x, $y $a => $dist ($d)\n";# if $x == 33 & $y == 2;

	return unless $a && $dist < $a;
    #print "Written\n";# if $x == 33 & $y == 2;
    #print "Set: $sx, $sy, $x, $y $a => $dist ($d)\n";# if $x == 33 & $y == 2;

    $im->[ $x + $y * $w ] = $dist;
    write_pixels( $im, $sx, $sy, $x, $y, $w, $h );
}

for my $f ( glob "[0-9][0-9][0-9].png" ) {
    fix($f);
}

#! /usr/bin/perl 
use strict;
use warnings;

use Image::Magick;
use Data::Dumper;

sub fix {
    my $image = shift;
    my $im    = Image::Magick->new;

    my ( $max_x, $max_y, $total_x, $total_y, $sum_x, $sum_y ) =
      ( 0, 0, 0, 0, 0, 0 );
    my $i = $im->Read($image);
    my ( $h, $w ) = $im->Get( 'height', 'width' );
    my $min_x = $w;
    my $min_y = $h;
    for my $x ( 0 .. $w ) {
        for my $y ( 0 .. $h ) {
            my ( $r, $g, $b ) = $im->GetPixel(
                x         => $x,
                y         => $y,
                normalize => 'false'
            );
            if ( $r || $g || $b ) {
                $sum_x += $x;
                $sum_y += $y;
                $total_x++;
                $total_y++;
                $min_x = $x if $x < $min_x;
                $min_y = $y if $y < $min_y;
                $max_x = $x if $x > $max_x;
                $max_y = $y if $y > $max_y;
            }
        }
    }

    my $ix     = int( $sum_x / $total_x ) - $min_x;
    my $iy     = int( $sum_y / $total_y ) - $min_y;
    my $size_x = $max_x - $min_x + 1;
    my $size_y = $max_y - $min_y + 1;
    my $copy   = Image::Magick->new( size => $size_x . 'x' . $size_y );
    $copy->Read('NULL:black');
	$copy->Color('black');
    my $dx    = 0;
    my $white = [ 255, 255, 255 ];

    for my $x ( $min_x .. $max_x ) {
        my $dy = 0;
        for my $y ( $min_y .. $max_y ) {
            my ( $r, $g, $b ) =
              $im->GetPixel( x => $x, y => $y, normalize => 'false' );
            if ( $r || $g || $b ) {
                $copy->SetPixel( x => $dx, y => $dy, color => $white );
            }
            $dy++;
        }
        $dx++;
    }

    $copy->Quantize(
        colors              => 2,
        colorspace          => 'Gray',
		'transparent-color' => '#FFF',
    );
    $copy->Comment("$min_x,$min_y,$ix,$iy");
    my $x = $copy->Write($image);
    warn $x if $x;
}

for my $f ( glob "[0-9]*.png" ) {
    fix($f);
}

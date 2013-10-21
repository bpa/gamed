#! /usr/bin/perl 
use strict;
use warnings;

use Image::Magick;
use Data::Dumper;

sub fix {
    my $image = shift;
    my $im    = Image::Magick->new;
    $im->Read($image);
    my ( $h, $w ) = $im->Get( 'height', 'width' );

    my $copy = Image::Magick->new( size => $w . 'x' . $h );
    $copy->Read('NULL:black');
    $copy->Color('black');
    my $black = [ 0,   0,   0 ];
    my $white = [ 255, 255, 255 ];

    for my $x ( 0 .. $w ) {
        for my $y ( 0 .. $h ) {
            my ( $alpha ) =
              $im->GetPixel( x => $x, y => $y, channel => 'alpha' );

            if ( $alpha == 1 ) {
                $copy->SetPixel( x => $x, y => $y, color => $black );
            }
            else {
                $copy->SetPixel( x => $x, y => $y, color => $white );
            }
        }
    }

    $copy->Quantize(
        colors              => 2,
        colorspace          => 'Gray',
        'transparent-color' => '#FFF',
    );
    my $x = $copy->Write($image);
    warn $x if $x;
}

for my $f ( glob "c[0-9]*.png" ) {
    fix($f);
}

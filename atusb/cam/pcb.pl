#!/usr/bin/perl

$d = 2.54/1000*12;
$r = $d/2+0.1;


sub orig
{
    $x0 = $_[0];
    $y0 = $_[1];
}


sub mil
{
    return $_[0]/1000*25.4;
}


sub cut
{
    if (defined $x) {
	if ($x == $_[0]+$x0 && $y == $_[1]+$y0) {
	    shift @_;
	    shift @_;
	} else {
	    print "\n";
	}
    }
    while (@_) {
	$x = shift @_;
	$y = shift @_;
#	($x, $y) = (-$y, $x);
	$x += $x0;
	$y += $y0;
	print "$x $y $z\n";
    }
}


sub one
{
    &cut(
      &mil(   0),	&mil(  0),
      &mil(   0),	&mil(640),
      &mil(1305),	&mil(640),
      &mil(1305),	&mil(  0),
      &mil(   0),	&mil(  0));
}


$z = -0.8;
# x: corner offset, compensation for rotation, array position
# y: corner offet
&orig(25+36*1, 2+17*0)
&one;

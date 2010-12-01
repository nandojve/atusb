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
	($x, $y) = (-$y, $x);
	$x += $x0;
	$y += $y0;
	print "$x $y $z\n";
    }
}


sub one
{
    &cut(
      &mil(   0),	&mil(  0),
      &mil(   0),	&mil(620),
      &mil( 810),	&mil(620),
      &mil( 850)  ,	&mil(580),
      &mil( 890),	&mil(620),
      &mil(1300),	&mil(620),
      &mil(1490),	&mil(430),
      &mil(1630),	&mil(430),
      &mil(1630),	&mil(360),
      &mil(1850),	&mil(360),
      &mil(1850),	&mil(430),
      &mil(2150),	&mil(430),
      &mil(2150),	&mil(-70),
      &mil( 890),	&mil(-70),
      &mil( 890),	&mil(  0),
      &mil( 850),	&mil( 40),
      &mil( 810),	&mil(  0),
      &mil(   0),	&mil(  0));
}


$z = -0.8;
# x: corner offset, compensation for rotation, array position
# y: corner offet
&orig(5+16+22*3, 5)
&one;

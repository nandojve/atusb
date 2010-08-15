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
      &mil(   0)-$r,	&mil(  0)-$r,
      &mil(   0)-$r,	&mil(620)+$r,
      &mil( 810)+$r,	&mil(620)+$r,
      &mil( 850)  ,	&mil(580)+$r,
      &mil( 890)-$r,	&mil(620)+$r,
      &mil(1300)+$r,	&mil(620)+$r,
      &mil(1490)+$r,	&mil(430)+$r,
      &mil(1630)+$r,	&mil(430)+$r,
      &mil(1630)+$r,	&mil(360)+$r,
      &mil(1850)-$r,	&mil(360)+$r,
      &mil(1850)-$r,	&mil(430)+$r,
      &mil(2150)+$r,	&mil(430)+$r,
      &mil(2150)+$r,	&mil(-70)-$r,
      &mil( 890)-$r,	&mil(-70)-$r,
      &mil( 890)-$r,	&mil(  0)-$r,
      &mil( 850)   ,	&mil( 40)-$r,
      &mil( 810)+$r,	&mil(  0)-$r,
      &mil(   0)-$r,	&mil(  0)-$r);
}


$z = -0.8
# x: corner offset, compensation for rotation, array position
# y: corner offet
&orig(5+16+22*2, 5)
&one;

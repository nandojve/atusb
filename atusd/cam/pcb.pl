#!/usr/bin/perl

$d = 2.54/1000*12;
$r = $d/2+0.25;


sub orig
{
    $x0 = $_[0];
    $y0 = $_[1];
}


sub mil
{
    return $_[0]/1000*25.4;
}


sub same
{
    return @_;
}


sub rot
{
    return (-$x, -$y);
}


sub cut
{
    local (*fn) = $_[0];

    shift @_;
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
	($x, $y) = &fn($x, $y);
	$x += $x0;
	$y += $y0;
	print "$x $y $z\n";
    }
}


sub one
{
    local ($fn) = @_;

    &cut($fn,
      &mil(   0)-$r,	&mil(   0)-$r,
      &mil(   0)-$r,	&mil(1030)+$r,
      &mil(1340)+$r,	&mil(1030)+$r,
      &mil(1340)+$r,	&mil( 640)-$r,
      &mil( 440)+$r,	&mil( 640)-$r,
      &mil( 440)+$r,	&mil( 390)-$r,
      &mil( 410)+$r,	&mil( 360)-$r,
      &mil( 410)+$r,	&mil( 310)+$r,
      &mil( 440)+$r,	&mil( 310)+$r,
      &mil( 440)+$r,	&mil( 250)-$r,
      &mil( 380)+$r,	&mil( 190)-$r,
      &mil( 380)+$r,	&mil(   0)-$r,
      &mil(   0)-$r,	&mil(   0)-$r);
}


$z = -0.8;
$col = 0;
$row = 1;
# x: corner offset, compensation for rotation, array position
# y: corner offet
&orig(5+54*$col, 5+33*$row)
&one(*same);
&orig(5+54*$col+15+&mil(1340), 5+33*$row+&mil(1030));
&one(*rot);

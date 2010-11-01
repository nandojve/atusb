#!/usr/bin/perl

$PI = atan2(1, 1)*4;

$d = 25.4/1000*35;
$r = $d/2-0.1;	# compensate deflection of board
$steps = 24;


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


# 0 is at "noon", rotating counter-clockwise

sub arc 
{
    local ($xc, $yc, $d, $a0, $a1) = @_;
    local ($rr) = $d/2;
    local $n = int(abs($a1-$a0)/360*$steps+0.5);

    $rr = 0 if $rr < 0;
    for ($i = 0; $i <= $n; $i++) {
	my $a = ($a0+($a1-$a0)/$n*$i)*$PI/180;
	$x = $x0+$xc-$rr*sin($a);
	$y = $y0+$yc+$rr*cos($a);
	print "$x $y $z\n";
    }
}


sub circ
{
    local ($xc, $yc, $d) = @_;

    print "\n";
    &arc($xc, $yc, $d, 0, 360);
    &circ($xc, $yc, $d-$r*2) if $d > $r*2;
}


sub hhole
{
    local ($xc0, $xc1, $yc, $d) = @_;
    local ($rr) = $d/2;

    &cut($xc0, $yc+$rr, $xc1, $yc+$rr);
    &arc($xc1, $yc, $d, 0, -180);
    undef $x;
    &cut($xc1, $yc-$rr, $xc0, $yc-$rr);
    &arc($xc0, $yc, $d, 180, 0);
}


sub pcb
{
    &cut(
      &mil(   0), &mil(   0),
      &mil(1180), &mil(   0),
      &mil(1180), &mil( 240),
      &mil(1000), &mil( 240),
      &mil(1000), &mil( 380),
      &mil(1180), &mil( 380),
      &mil(1180), &mil( 620),
      &mil(   0), &mil( 620),
      &mil(   0), &mil(   0));
}


sub holes
{
    # x-x0, y0-y, diameter
    &circ(&mil(3130-3020), &mil(3520-3122), &mil(43));
    &circ(&mil(3130-3020), &mil(3520-3298), &mil(43));

    &hhole(&mil(3100-3020), &mil(3159-3020), &mil(3520-2986), &mil(39));
    &hhole(&mil(3100-3020), &mil(3159-3020), &mil(3520-3434), &mil(39));
}




$z = -0.8;	# full thickness of board
# x: corner offset, compensation for rotation, array position
# y: corner offet

&orig(35*1, 45);

$r = $d/2;	# no compensation. don't wanna risk making holes too big.
&holes;

$r = $d/2-0.1;	# compensate deflection of board
&pcb;

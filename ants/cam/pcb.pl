#!/usr/bin/perl

$PI = atan2(1, 1)*4;

$d = 25.4/1000*35;


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


sub pcb
{
    &cut(
      &mil(   0), &mil(   0),
      &mil(  $W), &mil(   0),
      &mil(  $W), &mil( 490),
      &mil(   0), &mil( 490),
      &mil(   0), &mil(   0));
}


#
# board width
#
# antenna factor	width (mil)
#
#  80%			530
#  90%			565
# 100%			605
# 105%			620
# 110%			640
# 115%			658
# 120%			680
#

$W = 565;

$z = -0.8;	# full thickness of board
# x: corner offset, compensation for rotation, array position
# y: corner offet

&orig(20*2+3, 15*2+23);

$r = $d/2-0.1;	# compensate deflection of board
&pcb;

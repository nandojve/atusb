#!/usr/bin/perl
#
# hmac.pl - Simple macro pre-processor for HTML
#
# Written 2001 by Werner Almesberger
# Copyright 2001 EPFL DSC-ICA, Network Robots
#

#------------------------------------------------------------------------------
#
# hmac processes macro definitions of the following type:
#
# <MACRO NAME=name param[=default] ...> ... </MACRO>
#
# The macro is invoked with  <name param=value ...>  or with
# <name param=value ...> ... </name>
#
# HTML tags corresponding to the parameters (e.g. <param>) are expanded in
# the macro body. Parameters for which no default value was given must be
# specified.
#
# In the block-style invocation, the content of the block is assigned to
# the parameter BODY. Block-style invocations cannot be nested.
#
# Macros and macro definitions are processed in the order in which they appear
# in the input file. Macro definitions inside macros are processed when the
# outer macro is expanded.
#
# Files can be included with the <INCLUDE FILE=name> tag. File inclusions
# are made as we go.
#
# hmac is not case-sensitive.
#
#------------------------------------------------------------------------------

#
# Marker
#
$BM = "\001";
$EM = "\002";

#
# Collect -Dmacro=value options
#
while ($ARGV[0] =~ /^-D([^=]+)=/) {
    $mac{$1} = $';
    shift(@ARGV);
}

#
# Complain about unrecognized options
#
if ($ARGV[0] =~ /^-/) {
    print STDERR "usage: $0 [-Dmacro=value ...] file ...\n";
    exit(1);
}

#
# Read input and put warning
#
$in = join("",<>);
$in =~ s/\n/\n\n<!-- MACHINE-GENERATED FILE, DO NOT EDIT ! -->\n\n/;

#
# Scan text for macros or includes
#
while (1) {
    $first_macdef = &find_macdef($in);
    $first_macro = &find_macro($in);
    $first_include = &find_include($in);
    last if $first_macdef == -1 && $first_macro == -1 && $first_include == -1;
    if ($first_include != -1 &&
      ($first_include < $first_macro || $first_macro == -1) &&
      ($first_include < $first_macdef || $first_macdef == -1)) {
	$in = &include_file($in);
    }
    else {
	if (($first_macdef < $first_macro && $first_macdef != -1) ||
	  $first_macro == -1) {
	    $in = &define_macro($in);
	}
	else {
	    $in = &expand_macro($in);
	}
    }
}
print $in;


#
# Find includes, and macros and their definitions
#

sub find_include
{
    local ($in) = @_;
    return -1 unless $in =~ /<INCLUDE\s+file="[^"]*"/i;
    return length($`)+length($&);
}


sub find_macdef
{
    local ($in) = @_;
    return -1 unless $in =~ /<MACRO\b/i;
    return length $`;
}


sub find_macro
{
    local ($in) = @_;
    local ($first) = -1;

    for $mac (keys %mac) {
	if ($in =~ /<$mac\b/i) {
	    $first = length $` if $first == -1 || length $` < $first;
	}
    }
    return $first;
}


#
# Include a file
#

sub include_file
{
    local ($in) = @_;
    local ($name,$f);

    $in =~ /<INCLUDE\s+FILE=("([^"]*)"|\S+)\s*>\s*/i;
    $name = defined $2 ? $2 : $1;
    undef $f;
    open(FILE,$name) || die "open $name: $!";
    $f = $`.join("",<FILE>).$';
    close FILE;
    return $f;
}


#
# Extract first macro definition
#

sub define_macro
{
    local ($in) = @_;
    local ($a,$b,$c,$d);

    $in =~ s/<MACRO\b/$BM/gi;
    $in =~ s|</MACRO>|$EM|gi;
    if ($in =~ /$BM(("[^"]*"|[^>])*)>/is) {
	($a,$b,$c) = ($`,$1,$');
	$d = "";
	$need = 1;
        while ($need) {
	    $bm = index($c,$BM);
	    $em = index($c,$EM);
	    die "<MACRO> without </MACRO>" if $em == -1;
	    if ($bm < $em && $bm != -1) {
		$d .= substr($c,0,$bm+1,"");
		$need++;
	    }
	    else {
		$d .= substr($c,0,$em+1,"");
		$need--;
	    }
	}
	$c =~ s/^\s*//s;
	$in = $a.$c;
	chop($d);  # remove last $EM
	undef $name;
	undef %arg;
	$b =~ s/^\s*//;
	while ($b =~ /^([a-z_][a-z0-9_]*)(=("([^"]*)"|\S+))?\s*/is) {
	    $b = $';
	    ($prm = $1) =~ tr/a-z/A-Z/;
	    if ($prm eq "NAME") {
		die "duplicate NAME" if defined $name;
		die "NAME without value" unless defined $2;
		($name = defined $4 ? $4 : $3) =~ tr/a-z/A-Z/;
		next;
	    }
	    die "reserved parameter name BODY" if $prm eq "BODY";
	    die "duplicate parameter \"$prm\"" if exists $arg{$prm};
	    $arg{$prm} = defined $2 ? defined $4 ? $4 : $3 : undef;
	}
	die "syntax error" unless $b eq "";
	die "NAME parameter is missing" unless defined $name;
	$d =~ s/$BM/<MACRO/gi;
	$d =~ s|$EM|</MACRO>|gi;
	$mac{$name} = $d;
	$args{$name} = { %arg };
    }
    else {
	die "</MACRO> without <MACRO>" if $in =~ m|$EM|;
    }
    $in =~ s/$BM/<MACRO/gi;
    $in =~ s|$EM|</MACRO>|gi;
    return $in;
}


#
# Expand first macro
#

sub expand_macro
{
    local ($in) = @_;
    local ($found,$a,$b,$c);

    undef $a;
    for $mac (keys %mac) {
	if ($in =~ /<$mac\b(("[^"]*"|[^>])*)>/is) {
	    ($a,$b,$c) = ($`,$1,$') if length $` < length $a || !defined $a;
	}
	next unless defined $a;
	undef %arg;
	%arg = %{ $args{$mac} };
	if ($c =~ m|</$mac>|i) {
	    $arg{"BODY"} = $`;
	    $c = $';
	}
	else {
	    $arg{"BODY"} = undef;
	}
	$b =~ s/^\s*//;
	while ($b =~ /^([a-z_][a-z0-9_]*)(=("([^"]*)"|\S+))\s*/is) {
	    $b = $';
	    ($prm = $1) =~ tr/a-z/A-Z/;
	    die "unrecognized parameter \"$prm\"" unless exists $arg{$prm};
	    $arg{$prm} = defined $2 ? defined $4 ? $4 : $3 : undef;
	}
	$b = $mac{$mac};
	while (1) {
	    $done = 1;
	    for (keys %arg) {
		while ($b =~ /<$_>/i) {
		    $done = 0;
		    die "required parameter $_ is missing in macro $mac"
		      unless defined $arg{$_};
		    $b = $`.$arg{$_}.$';
		}
	    }
	    last if $done;
	}
	return $a.$b.$c;
    }
    return $in;
}

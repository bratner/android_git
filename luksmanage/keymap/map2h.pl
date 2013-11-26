#!/usr/bin/perl -w
use warnings;
use strict;



print "static struct keymap generated[]={\n";
while (my $line=<>){
	if ( $line=~ /area shape="rect" coords="(.+?)" href="(.+)"/ ){
		my ($x1,$y1,$x2,$y2)=split(',',$1);
		my $char=$2;
		if ($char eq '"') { $char='\"' };
		if ($char eq '\\') { $char='\\\\'};
		print "\t{ $x1, $y1, $x2, $y2, \"$char\"},\n";
		
	}
}
print "};\n"

# $Id$

# convert host.h to host.sno
# not in snobol4 because we want to use host.sno
# from timing.sno, which is part of the regression tests
# (but timing could be split off)

BEGIN {
	print "** THIS FILE IS MACHINE GENERATED DO NOT EDIT!!"
	print "** created from", FILENAME
	print "** by $Id$"
	print "**"
	print "** perhaps values should be in a FROZEN() TABLE()?"
	print ""
}

# note comment start
/\/\*/ {
	incomment = 1
}
# in comment; copy line, unless it has @@@ in it
incomment != 0 {
	if ($0 !~ /@@@/) print "*", $0
}
# note end of comment (after start and copy)
/\*\// {
	incomment = 0
}
# work!
/^#define/ {
	printf "\t%s = %s\n", $2, $3
}
# copy blank lines
/^$/ {
	print
}

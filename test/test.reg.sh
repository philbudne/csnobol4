#!/bin/sh

# test for "regular" files
#	captures stdout with no post-processing

PROG=$1
BASE=`basename $PROG .sno`
STATUS=1
#
if $SNOBOL -r $PROG > ${BASE}.tmp 2>/dev/null; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
	fi
fi
#
rm -f ${BASE}.tmp
#
exit $STATUS


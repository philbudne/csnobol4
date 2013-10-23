#!/bin/sh

# test with input file
#	captures stdout with no post-processing

PROG=$1
# BASE passed in environment
STATUS=1
#
if $SNOBOL $ARGS $PROG < ${BASE}.in > ${BASE}.tmp 2>/dev/null; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS


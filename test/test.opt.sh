#!/bin/sh

# test for "optional" files
#	captures stdout with no post-processing

PROG=$1
BASE=`echo $PROG | sed s/.sno//`
STATUS=2
#
if $SNOBOL $ARGS -r $PROG > ${BASE}.tmp 2>/dev/null; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

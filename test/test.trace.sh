#!/bin/sh

# test for files with TRACE() output
#	mash time = nnn

PROG=$1
BASE=`basename $PROG .sno`
STATUS=1
#
if $SNOBOL $ARGS -r $PROG 2>/dev/null | \
    sed 's/time = [0-9]*/time = xxx/' > ${BASE}.tmp; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

#!/bin/sh

# test for files with DUMP()/&DUMP=1 output
#	mash MAXLNGTH = nnn

PROG=$1
BASE=`basename $PROG .sno`
STATUS=1
#
if $SNOBOL $ARGS -r $PROG 2>/dev/null | \
    sed 's/MAXLNGTH = [0-9]*/MAXLNGTH = xxx/' > ${BASE}.tmp; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

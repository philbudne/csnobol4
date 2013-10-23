#!/bin/sh

# test for files with DUMP()/&DUMP=1 output
#	mash MAXLNGTH = nnn

PROG=$1
BASE=`echo $PROG | sed s/.sno//`
STATUS=1
#
if [ "$BLOCKS" ]; then
    # darn! couldn't get spaces to work here!
    FILL="-e /^&FILL.=.'/d"
fi
if $SNOBOL $ARGS -r $PROG 2>/dev/null | \
    sed -e 's/MAXLNGTH = [0-9]*/MAXLNGTH = xxx/' $FILL > ${BASE}.tmp; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

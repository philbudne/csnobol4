#!/bin/sh

# test for files with TRACE() output
#	mash time = nnn

# ~sigh~ wish I could use *SNOBOL*

PROG=$1
BASE=`echo $PROG | sed s/.sno//`
STATUS=1
#
if $SNOBOL $ARGS -r $PROG 2>/dev/null | \
    sed -e 's/time = [0-9\.e+-]*$/time = xxx/' > ${BASE}.tmp; then
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

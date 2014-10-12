#!/bin/sh

# test for "error" files
#	expects error exit; captures stderr

PROG=$1
BASE=`echo $PROG | sed s/.sno//`
STATUS=1
#
if $SNOBOL $ARGS -r $PROG > ${BASE}.tmp 2>&1; then
	true
else
	if cmp ${BASE}.ref ${BASE}.tmp; then
		STATUS=0
		rm -f ${BASE}.tmp
	fi
fi
#
#
exit $STATUS

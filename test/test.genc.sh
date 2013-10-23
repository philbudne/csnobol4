#!/bin/sh

# test for "regular" files
#	captures stdout with no post-processing

PROG=$1
BASE=`echo $PROG | sed s/.sno//`
STATUS=1
#
if $SNOBOL $PROG v311.sil > ${BASE}.tmp 2>/dev/null; then
	if cmp ${BASE}.ref ${BASE}.tmp && \
	   cmp proc.h.ref proc.h2 &&
	   cmp callgraph.ref callgraph &&
	   cmp static.h.ref static.h2; then
		STATUS=0
		rm -f ${BASE}.tmp proc.h2 static.h2 callgraph
	fi
fi
#
#
exit $STATUS


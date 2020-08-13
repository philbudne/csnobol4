#!/bin/sh
# $Id$

SDB=<SNOLIB_LIB>/sdb.sno
SNOBOL4=<BINDIR>/snobol4<VERS>

# handle user args??

if [ -d /tmp ]; then
    TMP=/tmp
else
    # Android
    TMP=.
fi
# create listing file, and pass filename in environment so sdb.sno can read it.
SDB_LISTFILE=$TMP/sdb_listing.$$
export SDB_LISTFILE

SDB_BREAKPOINTS=$TMP/sdb_bkpts.$$
export SDB_BREAKPOINTS

# remove listing/breakpoints files on exit (if not already removed by sdb)
trap "rm -f $SDB_LISTFILE $SDB_BREAKPOINTS" 0

while true; do
    $SNOBOL4 -b -l $SDB_LISTFILE -L $SDB "$@"
    STATUS=$?
    if [ ! -f "$SDB_BREAKPOINTS" ]; then
	exit $STATUS
    fi
done


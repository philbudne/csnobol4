#!/bin/sh
# $Id$

SDB=<SNOLIB_DIR>/sdb.sno
SNOBOL4=<BINDIR>/snobol4<VERS>

# handle user args??

# create listing file, and pass filename in environment so sdb.sno can read it.
SDB_LISTFILE=/tmp/sdb$$
export SDB_LISTFILE
$SNOBOL4 -b -l $SDB_LISTFILE -M $SDB $*

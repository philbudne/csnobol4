#!/bin/sh

if [ $# -gt 0 ]; then
	SNOBOL=$1
	if [ ! -x $SNOBOL ]; then
		echo "bad binary $SNOBOL" 1>&2
		exit 1
	fi
else
    SNOBOL=../xsnobol4
    if [ ! -x $SNOBOL ]; then
	SNOBOL=../snobol4
	if [ ! -x $SNOBOL ]; then
		echo 'No SNOBOL binary to test?' 1>&2
		exit 1
	fi
    fi
fi
echo "testing $SNOBOL"
export SNOBOL

# test list
TESTS=tests.in
echo "tests from $TESTS"

# function (ie; test or ref)
FUNC=test
# pass count
PASS=0
# fail count
FAIL=0

while read TYPE PROG; do
	if [ "$TYPE" = '#' ]; then
		continue
	fi
	echo ${PROG}:
	# XXX cope here with comments, empty lines?
	if ./${FUNC}.${TYPE}.sh $PROG; then
		PASS=`expr $PASS + 1`
		echo '	' passed.
	else
		FAIL=`expr $FAIL + 1`
		echo '	' failed.
		## sigh, can't seem to pass variables
		## out to top level, so just exit from here!
		exit $FAIL
	fi
done < $TESTS

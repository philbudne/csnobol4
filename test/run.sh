#!/bin/sh

if [ $# -gt 0 ]; then
	SNOBOL=$1
	if [ ! -x $SNOBOL ]; then
		echo "bad binary $SNOBOL" 1>&2
		exit 1
	fi
	shift
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

ARGS="$*"
echo "testing $SNOBOL $ARGS"
export SNOBOL ARGS

# test list
TESTS=tests.in
echo "tests from $TESTS"

# function (ie; test or ref)
FUNC=test
# pass count
PASS=0
# fail count
FAIL=0
# optional fail count
OPTFAIL=0

while read TYPE PROG; do
	if [ "$TYPE" = '#' ]; then
		continue
	fi
	echo ${PROG}:
	# XXX cope here with comments, empty lines?
	./${FUNC}.${TYPE}.sh $PROG
	case $? in
	0)
		PASS=`expr $PASS + 1`
		echo '	' passed.
		;;
	2)
		OPTFAIL=`expr $OPTFAIL + 1`
		echo '	failed (optional).'
		# keep going....
		;;
	*)
		echo '	' failed.
		## sigh, can't seem to pass variables
		## out to top level, so just exit from here!
		exit $FAIL
	esac
done < $TESTS
# would love to output counts, but "while" loop runs in a subshell(?)
# so variable changes made inside loop are not available!!!
#echo $PASS tests passed, $OPTFAIL optional tests failed.

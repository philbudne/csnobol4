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

IARGS="$*"
echo "testing $SNOBOL $IARGS"
echo 'search path:' `$SNOBOL $IARGS -z`
export SNOBOL

# test list
TESTS=tests.in

# function (ie; test or ref)
FUNC=test
# pass count
PASS=0
# fail count
FAIL=0
# optional fail count
OPTFAIL=0

if [ "$BLOCKS" ]; then
	TESTS="$TESTS blocks.in"
fi
echo "tests from $TESTS"
cat $TESTS | while read TYPE PROG; do
	if [ "$TYPE" = '#' ]; then
		continue
	fi
	(
	    ARGS="$IARGS -b"
	    echo ${PROG}:
	    for X in ${PROG}; do
		case "$X" in
		-*) ARGS="$ARGS $X";;
		*=*) eval export $X;;
		*) PROG=$X; break;;
		esac
	    done
	    export ARGS
	    # XXX cope here with comments, empty lines?
	    ./${FUNC}.${TYPE}.sh $PROG
	)
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
		FAIL=`expr $FAIL + 1`
		## sigh, can't seem to pass variables
		## out to top level, so just exit from here!
		exit $FAIL
	esac
done

# would love to output counts, but "while" loop runs in a subshell (due
# to input redirection?), so variable changes made inside loop are not
# available!!!
#echo $PASS tests passed, $OPTFAIL optional tests failed.

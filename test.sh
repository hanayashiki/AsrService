#!/bin/bash

pip3 install gevent requests
PY="python3"
TOOL="APITest/autoapi.py"

TESTS="single_normal parallel empty bad_url big"

for test in $TESTS; do
	COMMAND="$PY $TOOL $test"
	echo $COMMAND
	$COMMAND
	if [ "$?" -ne '0' ]
	then
		echo "Something is wrong. Check Logs/."
	fi
done

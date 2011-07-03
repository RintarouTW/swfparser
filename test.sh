#!/bin/bash

for f in `ls $1*.swf`
do
	filename=`basename $f`
	#`./swfparser $f > log/$filename.json 2>&1`
	`./swfparser $f`
	if [ "$?" != "1" ]; then
		echo "testing $f - FAIL"
	else
		echo "testing $f - OK"
	fi
done

#!/bin/bash

make clean
make

RESULT=$?
if [ $RESULT == 0 ]; then
	echo build complete!
fi

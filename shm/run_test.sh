#!/bin/sh
set -e

if [ "$#" != "1" ]; then
	echo "$0 <data_size>"
	exit 1
fi

nohup ./sub > /tmp/sub.out 2>&1 &

sleep 1

./pub $1

tail -n1 /tmp/sub.out

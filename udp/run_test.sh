#!/bin/sh
set -e

if [ "$#" != "1" ]; then
	echo "$0 <data_size>"
	exit 1
fi

OUT_FILE=/tmp/sub_udp_`date "+%y_%m_%d_%H_%M_%S"`.out

nohup ./sub > ${OUT_FILE} 2>&1 &

sleep 3

./pub $1

sleep 1

grep "========" ${OUT_FILE}

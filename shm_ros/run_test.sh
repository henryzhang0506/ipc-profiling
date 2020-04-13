#!/bin/sh
set -e

if [ "$#" -lt "1" ]; then
	echo "$0 <data_size>"
	exit 1
fi

SIZE=$1
shift 1

OUT_FILE=/tmp/sub_shm_ros_`date "+%y_%m_%d_%H_%M_%S"`.out

nohup ./sub "${@}" > ${OUT_FILE} 2>&1 &

sleep 3

./pub $SIZE "${@}"

grep "========" ${OUT_FILE}

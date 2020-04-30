#!/bin/sh
set -e

if [ "$#" != "1" ]; then
	echo "$0 <data_size>"
	exit 1
fi

OUT_FILE_PREFIX=/tmp/sub_ros_`date "+%y_%m_%d_%H_%M_%S"`
sub_idx=1
while [ ${sub_idx} -le ${NUM_SUBS:-1} ]; do
    nohup ./sub __name:=ros_sub_${sub_idx} > ${OUT_FILE_PREFIX}_${sub_idx}.out 2>&1 &
    let sub_idx=${sub_idx}+1
done


sleep 3

./pub $1

sleep 1

grep -h "========" ${OUT_FILE_PREFIX}*

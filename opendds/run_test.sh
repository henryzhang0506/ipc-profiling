#!/bin/sh
set -e

if [ "$#" != "1" ]; then
	echo "$0 <data_size>"
	exit 1
fi

OUT_FILE=/tmp/sub_tcp_`date "+%y_%m_%d_%H_%M_%S"`.out

# start the repo, this should be run only once
#nohup DCPSInfoRepo -o simple.ior > /tmp/DCPSInfoRepo.out 2>&1 & 

# start subscriber
nohup ./subscriber -DCPSConfigFile rtps.ini > ${OUT_FILE} 2>&1 &

# start publisher
./publisher -DCPSConfigFile rtps.ini $1

sleep 1

grep "========" ${OUT_FILE}

ALL: tcp shm ros shm_ros udp

.PHONY: tcp
tcp:
	make -C tcp

.PHONY: shm
shm:
	make -C shm

.PHONY: ros
ros:
	make -C ros

.PHONY: shm_ros
shm_ros:
	make -C shm_ros

.PHONY: udp
udp:
	make -C udp

.PHONY: clean
clean:
	make -C tcp clean
	make -C shm clean
	make -C ros clean
	make -C shm_ros clean
	make -C udp clean

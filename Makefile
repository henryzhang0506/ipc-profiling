ALL: tcp shm

.PHONY: tcp
tcp:
	make -C tcp

.PHONY: shm
shm:
	make -C shm

.PHONY: clean
clean:
	make -C tcp clean
	make -C shm clean

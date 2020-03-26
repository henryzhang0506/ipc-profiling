#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include "common.h"

#define MAX_MSG_SIZE 1024000000

uint8_t* shmptr;
uint8_t recvBuf[MAX_MSG_SIZE]; // max 1GB size
uint8_t header[16];
int data_size;
int round = 0;
double sum = 0.0;
int pub_pid = 0;

void handler_calculate(int signum) {
  if (signum == SIGUSR1) {
    printf("========= SHM mean transport time for size(%d) is: %lf ms =========\n", data_size,
           sum / round);
    exit(1);
  }
}

void handler_receive(int signum) {
  if (signum == SIGUSR2) {

    char share_name[16];
    sprintf(share_name, "shm_%d", round);
    int fd = shm_open(share_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      perror("shm_open");
      return;
    }

    if (read(fd, header, 16) != 16) {
      perror("read fd error");
      return;
    }

    double sent_time = *((double*)header);
    data_size = *((int*)(header + 8));
    pub_pid = *((int*)(header + 12));

    void* addr = mmap(NULL, data_size + 16, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
      perror("mmap");
      return;
    }

    shmptr = static_cast<uint8_t*>(addr);
    memcpy(recvBuf, shmptr + 16, data_size);

    double recv_time = get_wall_time();
    //    printf("recv_time is: %lf\n", recv_time);
    double transport_time = recv_time - sent_time;
    round += 1;
    sum += transport_time * 1000;
    printf("Transport time is: %lf ms\n", transport_time * 1000);
    munmap(shmptr, data_size+16);
    close(fd);
    shm_unlink(share_name);
    // trigger next round
    kill(pub_pid, SIGUSR1);
  }
}

int main() {
  int pid = getpid();
  std::ofstream f(sub_pid_name);
  f << pid;
  f.close();

  signal(SIGUSR1, handler_calculate);
  signal(SIGUSR2, handler_receive);

  while (1) {
    sleep(1);
  }

  return 0;
}

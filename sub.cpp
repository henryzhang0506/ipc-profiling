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

uint8_t* shmptr;
int data_size;
int round = 0;
double sum = 0.0;

void handler_calculate(int signum) {
  if (signum == SIGUSR1) {
    printf("========= mean transport time for size(%d) is: %lf ms =========\n", data_size,
           sum / round);
    exit(1);
  }
}

void handler_receive(int signum) {
  if (signum == SIGUSR2) {
    int fd = shm_open(topic_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      perror("shm_open");
      return;
    }

    char header[12];
    if (read(fd, header, 12) != 12) {
      perror("read fd error");
      return;
    }

    double sent_time = *((double*)header);
    data_size = *((int*)(header + 8));

    //printf("data_size is: %d\n", data_size);
    //    printf("sent_time is: %lf\n", sent_time);

    void* addr = mmap(NULL, data_size + 12, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
      perror("mmap");
      return;
    }

    shmptr = static_cast<uint8_t*>(addr);
    uint8_t* tmpBuf = new uint8_t[data_size];
    memcpy(tmpBuf, shmptr + 12, data_size);

    double recv_time = get_wall_time();
    //    printf("recv_time is: %lf\n", recv_time);
    double transport_time = recv_time - sent_time;
    round += 1;
    sum += transport_time * 1000;
    printf("Transport time is: %lf ms\n", transport_time * 1000);
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

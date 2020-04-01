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
#include <iostream>
#include <string>
#include "common.h"

const char* alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
bool next_round = false;

bool file_exists(const char* filename) {
  if (access(filename, F_OK) != -1) {
    return true;
  } else {
    return false;
  }
}

void handler_next_round(int signum) {
  if (signum == SIGUSR1) {
    next_round = true;
  }
}

void* create_shared_memory(const char* name, size_t size, int& fdd) {
  int fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  int res = ftruncate(fd, size);
  if (res == -1) {
    perror("ftruncate");
    return NULL;
  }

  void* addr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
  fdd = fd;
  return addr;
}

uint8_t* create_tmp_data(int size) {
  uint8_t* tmpBuf = new uint8_t[size];
  for (int i = 0; i < size; ++i) {
    tmpBuf[i] = alphanumeric[rand() % 62];
  }
  return tmpBuf;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s <pub_size>\n", *argv);
    return 1;
  }

  int pid = getpid();

  // Receive signal for next round
  signal(SIGUSR1, handler_next_round);
  // wait for subscriber's pid file
  while (!file_exists(sub_pid_name)) {
    printf("waiting for sub_pid_name: %s\n", sub_pid_name);
    sleep(1);
  }

  // Get subscriber's pid
  std::ifstream f(sub_pid_name);
  int sub_pid;
  f >> sub_pid;

  int data_size = atoi(*(argv + 1));

  // Begin the test round
  for (int i = 0; i < GetNumRounds() + 100; ++i) {
    // time used in allocating memory do not included
    uint8_t* data = create_tmp_data(data_size);
    // next_round = false;
    double current_time = get_wall_time();
    char share_name[16];
    sprintf(share_name, "shm_%d", i);

    int fd = -1;
    uint8_t* shmptr = static_cast<uint8_t*>(create_shared_memory(share_name, data_size + 16, fd));

    // encapsulate timestamp
    memcpy(shmptr, &current_time, sizeof(current_time));
    // encapsulate data_size
    memcpy(shmptr + 8, &data_size, 4);
    // encapsulate pid
    memcpy(shmptr + 12, &pid, 4);
    memcpy(shmptr + 16, data, data_size);
    kill(sub_pid, SIGUSR2);

    // Release memory
    munmap(shmptr, data_size + 16);
    close(fd);
    // while (!next_round) {
    //  // Waiting receiver end
    //  usleep(1000);
    //}
    // shm_unlink(share_name);
    usleep(1000000 / GetFrequencyHZ());
  }
  kill(sub_pid, SIGUSR1);

  return 0;
}

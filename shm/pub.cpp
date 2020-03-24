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

#define ROUND 100
const char* alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

bool file_exists(const char* filename) {
  if (access(filename, F_OK) != -1) {
    return true;
  } else {
    return false;
  }
}

void* create_shared_memory(const char* name, size_t size) {
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
  uint8_t* data = create_tmp_data(data_size);

  // Begin the test round
  for (int i = 0; i < ROUND; ++i) {
      double current_time = get_wall_time();

      uint8_t* shmptr = static_cast<uint8_t*>(create_shared_memory(topic_name, data_size + 4));

      memcpy(shmptr, &current_time, sizeof(current_time));
      memcpy(shmptr+8, &data_size, sizeof(current_time));
      memcpy(shmptr + 12, data, data_size);
      kill(sub_pid, SIGUSR2);
  }
  sleep(1);
  kill(sub_pid, SIGUSR1);

  return 0;
}

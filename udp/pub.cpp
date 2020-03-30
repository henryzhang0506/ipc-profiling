#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include "common.h"
#define PORT 8080

#define MAXLINE 1024

const char* alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

uint8_t* create_tmp_data(int size) {
  uint8_t* tmpBuf = new uint8_t[size];
  for (int i = 0; i < size; ++i) {
    tmpBuf[i] = alphanumeric[rand() % 62];
  }
  return tmpBuf;
}

int main(int argc, char const* argv[]) {
  if (argc != 2) {
    printf("Usage: %s <pub_size>\n", *argv);
    return 1;
  }
  int data_size = atoi(*(argv + 1));
  int sockfd = 0;
  char buffer[MAXLINE];
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  int n, len;

  for (int r = 0; r < ROUND; ++r) {
    uint8_t* data = create_tmp_data(data_size);
    double cur_time = get_wall_time();
    n = sendto(sockfd, &cur_time, sizeof(double), 0, (const struct sockaddr*)&serv_addr,
               sizeof(serv_addr));
    if (n != sizeof(double)) {
      perror("Send sent_time error!\n");
      exit(1);
    }
    n = sendto(sockfd, &data_size, sizeof(int), 0, (const struct sockaddr*)&serv_addr,
               sizeof(serv_addr));
    if (n != sizeof(int)) {
      perror("Send data_size error!\n");
      exit(1);
    }
    int i = 0;
    int send_size = 32768;
    int remain_size = 0;
    while (i < data_size) {
      remain_size = std::min(32768, data_size - i); 
      n = sendto(sockfd, data + i, remain_size, 0, (const struct sockaddr*)&serv_addr,
                 sizeof(serv_addr));
      if (n < 0) {
        perror("Send data error!\n");
        exit(1);
      }
      i += n;
    }
    usleep(50000);
  }

  // for (int r = 0; r < ROUND; ++r) {
  //  // time in allocating memory not included
  //  printf("Round: %d\n", r);
  //  uint8_t* data = create_tmp_data(data_size);
  //  double current_time = get_wall_time();
  //  write(sock, &current_time, sizeof(double));
  //  write(sock, &data_size, 4);
  //  int count = 0;
  //  int i = 0;
  //  while (i < data_size) {
  //    count = write(sock, data + i, data_size - i);
  //    i += count;
  //  }
  //  delete[] data;
  //}

  return 0;
}

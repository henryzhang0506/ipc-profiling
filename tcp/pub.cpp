#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "common.h"
#define PORT 8080

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
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  int on = 1;

  //if (setsockopt(sock, SOL_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
    perror("setsockopt tcp_nodelay error");
    exit(EXIT_FAILURE);
  }
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  for (int r = 0; r < ROUND; ++r) {
    // time in allocating memory not included
    printf("Round: %d\n", r);
    uint8_t* data = create_tmp_data(data_size);
    double current_time = get_wall_time();
    write(sock, &current_time, sizeof(double));
    write(sock, &data_size, 4);
    int count = 0;
    int i = 0;
    while (i < data_size) {
      count = write(sock, data + i, data_size - i);
      i += count;
    }
    delete[] data;
    // Wait data complete in the connection
    usleep(50000);
  }

  return 0;
}

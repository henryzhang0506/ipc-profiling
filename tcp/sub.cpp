#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../common.h"
#define PORT 8080

#define MAX_BUF_LEN 1024000000  // 1GB
char recvBuf[MAX_BUF_LEN];

double sum = 0;

int main(int argc, char const *argv[]) {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  double sent_time;
  int data_size;

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  int on = 1;

  if (setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
    perror("setsockopt tcp_nodelay error");
    exit(EXIT_FAILURE);
  }

  for (int r = 0; r < GetNumRounds(); ++r) {
    if(read(new_socket, &sent_time, sizeof(double)) != 8) {
      perror("read sent_time error!");
      exit(1);
    }
    if (read(new_socket, &data_size, 4) != 4) {
      perror("read data_size error!");
      exit(1);
    }

    //double end_time1 = get_wall_time();

    int i = 0, count = 0;
    while (i < data_size) {
      count = read(new_socket, recvBuf + i, data_size - i);
      i += count;
    }
    double end_time = get_wall_time();
    double delta = (end_time - sent_time) * 1000;
    // printf("transport time is: %lf\n", delta);
    // sum += delta;

    // Send back the data
    i = 0, count = 0;
    while (i < data_size) {
      count = write(new_socket, recvBuf + i, data_size - i);
      i += count;
    }
    fprintf(stderr, "travel time is: %lf\n", delta);
  }
  fprintf(stderr, "data_size(%d) test end!\n", data_size);
  //printf("========= TCP mean transport time for size(%d) is: %lf ms =========\n", data_size,
         //sum / GetNumRounds());
  return 0;
}

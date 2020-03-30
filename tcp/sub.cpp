#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.h"
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
  for (int r = 0; r < ROUND; ++r) {
    read(new_socket, &sent_time, sizeof(double));
    read(new_socket, &data_size, sizeof(int));

    double end_time1 = get_wall_time();
    fprintf(stderr, "read header time is: %lf\n", (end_time1 - sent_time) * 1000);

    int i = 0, count = 0;
    while (i < data_size) {
      count = read(new_socket, recvBuf + i, data_size - i);
      i += count;
    }
    double end_time = get_wall_time();
    double delta = (end_time - sent_time) * 1000;
    fprintf(stderr, "end_time is: %lf, sent_time is: %lf\n", end_time, sent_time);
    printf("transport time is: %lf\n", delta);
    sum += delta;
  }
  printf("========= TCP mean transport time for size(%d) is: %lf ms =========\n", data_size,
         sum / ROUND);
  return 0;
}

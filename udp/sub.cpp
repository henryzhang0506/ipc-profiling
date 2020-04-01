#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
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
  int server_fd;
  struct sockaddr_in servaddr, cliaddr;
  int opt = 1;
  double sent_time;
  int data_size;

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  uint32_t len;
  len = sizeof(cliaddr);  // len is value/resuslt

  for (int r = 0; r < GetNumRounds(); ++r) {
    int i = 0;
    ssize_t n;
    recvfrom(server_fd, &sent_time, sizeof(double), MSG_WAITALL, NULL, NULL);
    recvfrom(server_fd, &data_size, sizeof(double), MSG_WAITALL, NULL, NULL);
    while (i < data_size) {
      n = recvfrom(server_fd, (char *)(recvBuf + i), data_size-i, MSG_WAITALL, NULL, NULL);
      if (n < 0) {
        // fprintf(stderr, "Received data size: %ld\n", n);
        perror("recvfrom error!\n");
        exit(1);
      }
      i += n;
      //fprintf(stderr, "Received data: %ld, offset: %d\n", n, i);
    }
    double end_time = get_wall_time();
    double delta = (end_time - sent_time) * 1000;
    sum += delta;
    fprintf(stderr, "UDP transport time for size(%d) is: %lf\n", data_size, delta);
  }
  // printf("cliaddr IP address: %s\n", s);
  // buffer[n] = '\0';

  // for (int r = 0; r < ROUND; ++r) {
  //  read(new_socket, &sent_time, sizeof(double));
  //  read(new_socket, &data_size, sizeof(int));

  //  int i = 0, count = 0;
  //  while (i < data_size) {
  //    count = read(new_socket, recvBuf + i, data_size - i);
  //    i += count;
  //  }
  //  double end_time = get_wall_time();
  //  double delta = (end_time - sent_time) * 1000;
  //  printf("transport time is: %lf\n", delta);
  //  sum += delta;
  //}
  printf("========= UDP mean transport time for size(%d) is: %lf ms =========\n", data_size,
         sum / GetNumRounds());
  return 0;
}

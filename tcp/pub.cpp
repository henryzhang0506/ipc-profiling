#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../common.h"
#define PORT 8080

const char* alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
double sum = 0.0;
int real_round = 0;

uint8_t* create_tmp_data(int size) {
  uint8_t* tmpBuf = new uint8_t[size];
  for (int i = 0; i < size; ++i) {
    tmpBuf[i] = alphanumeric[rand() % 62];
  }
  return tmpBuf;
}

int main(int argc, char const* argv[]) {
  if (argc > 3) {
    printf("Usage: %s <pub_size> [<sub address>]\n", *argv);
    return 1;
  }
  const char* dest = "127.0.0.1";
  if (argc == 3) {
    dest = argv[2];
  }
  int data_size = atoi(*(argv + 1));
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Socket creation error \n");
    return -1;
  }

  int on = 1;

  // if (setsockopt(sock, SOL_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
    perror("setsockopt tcp_nodelay error");
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
  if (inet_pton(AF_INET, dest, &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }
  int R = GetNumRounds();
  for (int r = 0; r < R + 10; ++r) {
    // time in allocating memory not included
    printf("Round: %d\n", r);
    uint8_t* data = create_tmp_data(data_size);
    double current_time = get_wall_time();
    if(write(sock, &current_time, sizeof(double)) != 8) {
      perror("Write current timestamp error!");
      exit(1);
    }
    if (write(sock, &data_size, 4) != 4) {
      perror("Write data_size error!");
      exit(1);
    }
    int count = 0;
    int i = 0;
    while (i < data_size) {
      count = write(sock, data + i, data_size - i);
      i += count;
    }

    // Receive the back data
    i = 0, count = 0;
    while (i < data_size) {
      count = read(sock, data + i, data_size - i);
      i += count;
    }
    double delta = (get_wall_time() - current_time) * 1000;
    if (r > 3 && r < R + 5) {
        fprintf(stderr, "Roundtrip time is: %lf\n", delta);
        sum += delta;
	real_round += 1;
    }
    delete[] data;
    // Wait data complete in the connection
    usleep(1000000 / GetFrequencyHZ());
  }
  fprintf(stderr, "========= Mean Roundtrip time for size(%d) is: %lf ms ========= \n", data_size,
          sum / real_round);
  return 0;
}

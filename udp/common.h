#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define ROUND 100

double get_wall_time() {
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    perror("gettimeofday error!");
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#define sub_pid_name "/tmp/SUB.pid"
#define pub_pid_name "/tmp/PUB.pid"
#define topic_name "share_memory_topic"

double get_wall_time() {
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    perror("gettimeofday error!");
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

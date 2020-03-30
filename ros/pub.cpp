#include <unistd.h>
#include <atomic>
#include <sstream>
#include "common.h"
#include "ros/ros.h"
#include "std_msgs/String.h"

const char* alphanumeric = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

uint8_t* create_tmp_data(int size) {
  uint8_t* tmpBuf = new uint8_t[size];
  for (int i = 0; i < size; ++i) {
    tmpBuf[i] = alphanumeric[rand() % 62];
  }
  return tmpBuf;
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "ros_pub");
  if (argc != 2) {
    printf("Usage: %s <pub_size>\n", *argv);
    return 1;
  }

  int data_size = atoi(*(argv + 1));
  ros::NodeHandle n;
  ros::Publisher perf_pub = n.advertise<std_msgs::String>("ros_testing", 1000);

  std_msgs::String msg;

  uint8_t* tmpData = create_tmp_data(data_size);
  msg.data.resize(12 + data_size);
  // For the first round
  int round = 0;
  while (ros::ok()) {
    ros::spinOnce();
    // Should send a few more messages, since ros may lose some messages at the begining
    if (round >= ROUND + 10) {
      ros::shutdown();
    }
    std::memcpy(&msg.data[8], &data_size, 4);
    std::memcpy(&msg.data[12], tmpData, data_size);

    double current_time = get_wall_time();
    std::memcpy(&msg.data[0], &current_time, 8);

    perf_pub.publish(msg);
    // fprintf(stderr, "Published message!\n");
    round += 1;
    // If transport time larger than 500ms, then can not use this to measure one e2e overhead
    usleep(500000);
  }
  return 0;
}

#include <unistd.h>
#include <atomic>
#include <sstream>
#include "common.h"
#include "ros/ros.h"
#include "std_msgs/String.h"


int main(int argc, char** argv) {
  ros::init(argc, argv, "ros_pub");
  if (argc != 2) {
    printf("Usage: %s <pub_size>\n", *argv);
    return 1;
  }

  int data_size = atoi(*(argv + 1));
  auto msg = GenerateProtoMessage(data_size);

  ros::NodeHandle n;
  ros::Publisher perf_pub = n.advertise<std_msgs::String>("ros_testing", 1000);

  // For the first round
  int round = 0;
  ros::Rate loop_rate(GetFrequencyHZ());
  while (ros::ok()) {
    ros::spinOnce();
    // Should send a few more messages, since ros may lose some messages at the begining
    if (round >= GetNumRounds() + 100) {
      ros::shutdown();
    }

    msg.set_publish_timestamp(ros::WallTime::now().toNSec());
    std_msgs::String msg_str;
    msg.SerializeToString(&msg_str.data);

    perf_pub.publish(msg_str);
    // fprintf(stderr, "Published message!\n");
    round += 1;
    // If transport time larger than 500ms, then can not use this to measure one e2e overhead
    loop_rate.sleep();
  }
  return 0;
}

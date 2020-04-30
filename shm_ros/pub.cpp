#include <gflags/gflags.h>
#include <ipc/publisher.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <unistd.h>
#include <atomic>
#include <sstream>

#include "common.h"

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  ros::init(argc, argv, "ros_pub");
  if (argc < 2) {
    printf("Usage: %s <pub_size>\n", *argv);
    return 1;
  }

  SetupMetrics();
  SetupShmFlags();

  int data_size = atoi(*(argv + 1));
  auto msg = GenerateProtoMessage(data_size);
  ros::NodeHandle n;
  auto perf_pub =
      drive::common::ipc::advertise<decltype(msg)>(n, "ros_shm_topic", 1000);

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

    perf_pub.publish(msg);
    round += 1;
    loop_rate.sleep();
  }

  drive::common::metrics::Registry::GetInstance()->get_forwarding_reporter()->stop();
  return 0;
}

#include <gflags/gflags.h>
#include <unistd.h>
#include <ipc/subscriber.h>
#include <atomic>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "common.h"

double sum = 0.0;
int cnt = 0;

class Wrapper {
 public:
  Wrapper() {
    sub_ = drive::common::ipc::subscribe(n_, "ros_shm_topic", 1000, &Wrapper::callback, this);
  }

  void callback(const drive::common::ipc::ShmMessage& msg) {
    if (!ros::ok()) {
        return;
    }
    auto transport_time_ms = (ros::WallTime::now().toNSec() - msg.publish_timestamp()) * 1.0e-6;
    printf("SHM_ROS transport time is: %lf ms\n", transport_time_ms);
    sum += transport_time_ms;
    cnt += 1;
    if (cnt >= GetNumRounds()) {
      fprintf(stderr, "========= SHM_ROS mean transport time for size(%u) is: %lf ms =========\n",
              msg.payload().size(), sum / cnt);
      ros::shutdown();
      sub_ = {};
    }
  }

  ros::NodeHandle n_;
  drive::common::ipc::Subscriber sub_;
};

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  ros::init(argc, argv, "ros_sub");
  SetupShmFlags();
  SetupMetrics();
  Wrapper w;
  ros::spin();
  drive::common::metrics::Registry::GetInstance()->get_forwarding_reporter()->stop();
  return 0;
}

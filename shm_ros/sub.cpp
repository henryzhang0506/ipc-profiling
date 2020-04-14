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

  void callback(const std_msgs::String::ConstPtr& msg) {
    double cur_time = get_wall_time();
    double sent_time = *((double*)(msg->data.data()));
    int data_size = *((int*)(msg->data.data() + 8));
    double delta = (cur_time - sent_time) * 1000;
    printf("SHM_ROS transport time is: %lf ms\n", delta);
    sum += delta;
    cnt += 1;
    if (cnt >= GetNumRounds()) {
      fprintf(stderr, "========= SHM_ROS mean transport time for size(%d) is: %lf ms =========\n",
              data_size, sum / cnt);
      //exit(0);
      ros::shutdown();
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

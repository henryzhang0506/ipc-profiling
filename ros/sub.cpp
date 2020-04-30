#include <unistd.h>
#include <atomic>
#include "common.h"
#include "ros/ros.h"
#include "std_msgs/String.h"

double sum = 0.0;
int cnt = 0;

class Wrapper {
 public:
  Wrapper() {
    //sub_ = n_.subscribe("ros_testing", 1000, &Wrapper::callback, this,
    //                    ros::TransportHints().tcpNoDelay(true).reliable());
    sub_ = n_.subscribe("ros_testing", 1000, &Wrapper::callback, this);
  }

  void callback(const std_msgs::String::ConstPtr& msg) {
    drive::common::ipc::ShmMessage shm_msg;
    if (!ros::ok() || !shm_msg.ParseFromString(msg->data)) {
        return;
    }
    auto transport_time_ms = (ros::WallTime::now().toNSec() - shm_msg.publish_timestamp()) * 1.0e-6;
    printf("ROS transport time is: %lf ms\n", transport_time_ms);
    sum += transport_time_ms;
    cnt += 1;
    if (cnt > GetNumRounds()) {
      fprintf(stderr, "========= ROS mean transport time for size(%lu) is: %lf ms =========\n",
              shm_msg.payload().size(), sum / cnt);
      ros::shutdown();
      sub_ = {};
    }
  }

  ros::NodeHandle n_;
  ros::Publisher pub_;
  ros::Subscriber sub_;
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "ros_sub");
  Wrapper w;
  ros::spin();
  return 0;
}

#include <unistd.h>
#include <atomic>
#include "../common.h"
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
    double cur_time = get_wall_time();
    double sent_time = *((double*)(msg->data.data()));
    int data_size = *((int*)(msg->data.data() + 8));
    double delta = (cur_time - sent_time) * 1000;
    printf("ROS transport time is: %lf ms\n", delta);
    sum += delta;
    cnt += 1;
    if (cnt > GetNumRounds()) {
      fprintf(stderr, "========= ROS mean transport time for size(%d) is: %lf ms =========\n",
              data_size, sum / cnt);
      ros::shutdown();
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

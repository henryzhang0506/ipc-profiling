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
    FLAGS_ipc_pubsub_report_internal_metrics = false;
    FLAGS_ipc_pubsub_subscriber_modes = "shm";
    FLAGS_ipc_pubsub_subscriber_protocol = "tcp";
    FLAGS_ipc_pubsub_subscriber_tcp_nodelay = "true";
    FLAGS_ipc_pubsub_subscriber_ros_connection_management_mode = "off";

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
    if (cnt > ROUND) {
      fprintf(stderr, "========= ROS mean transport time for size(%d) is: %lf ms =========\n",
              data_size, sum / cnt);
      //exit(0);
      ros::shutdown();
    }
  }

  ros::NodeHandle n_;
  drive::common::ipc::Subscriber sub_;
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "ros_sub");
  Wrapper w;
  // Precision 0.1ms
  ros::Rate loop_rate(100000);
  while (ros::ok()) {
    ros::spinOnce();
    loop_rate.sleep();
  }

  // ros::spin();
  return 0;
}

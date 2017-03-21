#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "sensor_msgs/LaserScan.h"
ros::Publisher pub;
void fun_Callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	int num_letture=(msg->angle_max-msg->angle_min)/msg->angle_increment;
	float distanza_minima=msg->ranges[0];
	for(int i=1;i<num_letture;i++){
		if(distanza_minima>msg->ranges[i]){
			distanza_minima=msg->ranges[i];
			}
		}
	std_msgs::Float32 result;
	result.data=distanza_minima;
	pub.publish(result); 
  //ROS_INFO("La distanza minima e': %lf",distanza_minima);
  
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "min_laser");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("base_scan", 1000, fun_Callback);
	pub =n.advertise<std_msgs::Float32>("result_topic",1000);
	ros::spin();
	return 0;
 }

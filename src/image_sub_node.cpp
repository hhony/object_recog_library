#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include "detection.h"

cv_bridge::CvImagePtr cv_ptr;
image_transport::Publisher pub;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
	try {
		  cv_ptr = cv_bridge::toCvCopy(msg,"bgr8");
		  const cv::Mat * imagePtr = &(cv_ptr->image);

		  cv::Mat out_image;
		  //out_image = find_objects(imagePtr);
		  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", out_image).toImageMsg();
		  pub.publish(msg);
	  } catch (cv_bridge::Exception& e) {
	    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
	  }

	  std::cout << "." << std::endl;
}

void AddSampleforDetection(int id, int H_min,int S_min,int V_min,int H_max,int S_max,int V_max,
						double min_width, double max_width,double min_height,double max_height)
{
	std::vector<int> min{H_min,S_min,V_min};
    std::vector<int> max{H_max,S_max,V_max};
    register_sample(id,min,max,min_width,max_width,min_height,max_height);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "image_listener");
  ros::NodeHandle nh;
  ros::NodeHandle np("~");
  std::string topic;

  AddSampleforDetection(1,165,50,50,175,255,255,1,1,1,1); // Red hockey puck
  AddSampleforDetection(2,20,50,50,30,255,255,1,1,1,1);   // Yellow PVC pipe
  AddSampleforDetection(3,5,50,50,15,255,255,1,1,1,1);    // Orange PVC pipe
  AddSampleforDetection(4,90,60,60,110,255,255,1,1,1,1);  // White hooked sample
  AddSampleforDetection(5,0,50,50,5,255,255,1,1,1,1);     // Pink Tennis Ball

  std::cout << "sample size = " << getSampleSize() << std::endl;

  if(!np.getParam("topic",topic))
  {
    topic = "camera/image";
  }
  image_transport::ImageTransport it(topic);
  image_transport::Subscriber sub = it.subscribe(topic, 1, imageCallback);
  pub = it.advertise("chatter",1);
  ros::spin();
}

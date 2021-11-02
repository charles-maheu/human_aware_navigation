#include <ros/ros.h>
#include <people_msgs/People.h>
#include <geometry_msgs/Pose.h>
#include <visualization_msgs/Marker.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <string>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tf_to_person");

  ros::NodeHandle priv_n("~");
  ros::NodeHandle n;

  double x, y, phi, vel;

  visualization_msgs::Marker people_marker;
  people_marker.header.frame_id = "map";
  people_marker.ns = "model";
  people_marker.type = visualization_msgs::Marker::MESH_RESOURCE;
  people_marker.action = visualization_msgs::Marker::MODIFY;
  people_marker.mesh_resource = "package://social_nav_simulation/gazebo/models/human/meshes/walking.dae";
  people_marker.mesh_use_embedded_materials = true;
  people_marker.scale.x = 1.0;
  people_marker.scale.y = 1.0;
  people_marker.scale.z = 1.0;

  priv_n.param("x", x, 0.0);
  priv_n.param("y", y, 0.0);
  priv_n.param("phi", phi, 0.0);
  priv_n.param("vel", vel, 0.0);

  tf::TransformListener listener;

  ros::Publisher people_pub = n.advertise<people_msgs::People>("people", 1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("people_viz", 1);

  people_msgs::People people;
  people_msgs::Person person;

  ros::Duration(2.0).sleep();
  ROS_INFO("starting tf to people");

  ros::Time start = ros::Time::now();

  ros::Rate rate = ros::Rate(10);

  person.position.x = 0;
  person.position.x = 0;

  while(ros::ok())
  {
    people.people.clear();
    ros::Duration delta = (ros::Time::now() - start);
    double delta_t = delta.toSec();

    tf::StampedTransform transform;
    for(int i=0; i<=5; i++)
    {
      std::string person_frame = "Person_"+std::to_string(i);
      ROS_INFO("Checking for frame \"%s\" ", person_frame.c_str());

      try{
      listener.lookupTransform("map", person_frame,  
                               ros::Time(0), transform);
        if(transform.getOrigin().x() != person.position.x && transform.getOrigin().y() != person.position.x)
        {
          person.position.x = transform.getOrigin().x();
          person.position.y = transform.getOrigin().y();
        }
      }
      catch (tf::TransformException ex){
        ROS_ERROR("%s",ex.what());
        //ros::Duration(1.0).sleep();
      }
    }

    ROS_INFO("Detected pos (x=%f,y=%f) : ", person.position.x, person.position.y);
    
    person.velocity.x = vel * cos(phi);
    person.velocity.y = vel * sin(phi);
    //person.position.x = x + delta_t * person.velocity.x;
    //person.position.y = y + delta_t * person.velocity.y;

    people_marker.pose.position.x = person.position.x;
    people_marker.pose.position.y = person.position.y;
    tf::quaternionTFToMsg(tf::createQuaternionFromYaw(phi+M_PI_2), people_marker.pose.orientation);

    people.header.frame_id = "map";
    people.header.stamp = start + delta;
    people.people.push_back(person);

    people_pub.publish(people);
    marker_pub.publish(people_marker);
    rate.sleep();
  }

  return 0;
}

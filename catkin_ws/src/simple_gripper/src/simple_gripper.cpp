#include <ros/ros.h>
#include <pr2_controllers_msgs/Pr2GripperCommandAction.h>
#include <actionlib/client/simple_action_client.h>
#include <gazebo_msgs/GetModelState.h>
#include <gazebo_msgs/GetLinkState.h>
#include <tf/transform_listener.h>

// Our Action interface type, provided as a typedef for convenience
typedef actionlib::SimpleActionClient<pr2_controllers_msgs::Pr2GripperCommandAction> GripperClient;

class Gripper_Right{
private:
  GripperClient* gripper_client_;  

public:
  //Action client initialization
  Gripper_Right(){

    //Initialize the client for the Action interface to the gripper controller
    //and tell the action client that we want to spin a thread by default
    gripper_client_ = new GripperClient("r_gripper_controller/gripper_action", true);
    
    //wait for the gripper action server to come up 
    while(!gripper_client_->waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the r_gripper_controller/gripper_action action server to come up");
    }
  }

  ~Gripper_Right(){
    delete gripper_client_;
  }

  //Open the gripper
  void open(){
    pr2_controllers_msgs::Pr2GripperCommandGoal open;
    open.command.position = 0.08;
    open.command.max_effort = -1.0;  // Do not limit effort (negative)
    
    ROS_INFO("Sending open goal");
    gripper_client_->sendGoal(open);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The gripper opened!");
    else
      ROS_INFO("The gripper failed to open.");
  }

  //Close the gripper
  void close(){
    pr2_controllers_msgs::Pr2GripperCommandGoal squeeze;
    squeeze.command.position = 0.0;
    squeeze.command.max_effort = 50.0;  // Close gently
    
    ROS_INFO("Sending squeeze goal");
    gripper_client_->sendGoal(squeeze);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The gripper closed!");
    else
      ROS_INFO("The gripper failed to close.");
  }
};

class Gripper_Left{
private:
  GripperClient* gripper_client_;  

public:
  //Action client initialization
  Gripper_Left(){

    //Initialize the client for the Action interface to the gripper controller
    //and tell the action client that we want to spin a thread by default
    gripper_client_ = new GripperClient("l_gripper_controller/gripper_action", true);
    
    //wait for the gripper action server to come up 
    while(!gripper_client_->waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the l_gripper_controller/gripper_action action server to come up");
    }
  }

  ~Gripper_Left(){
    delete gripper_client_;
  }

  //Open the gripper
  void open(){
    pr2_controllers_msgs::Pr2GripperCommandGoal open;
    open.command.position = 0.08;
    open.command.max_effort = -1.0;  // Do not limit effort (negative)
    
    ROS_INFO("Sending open goal");
    gripper_client_->sendGoal(open);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The gripper opened!");
    else
      ROS_INFO("The gripper failed to open.");
  }

  //Close the gripper
  void close(){
    pr2_controllers_msgs::Pr2GripperCommandGoal squeeze;
    squeeze.command.position = 0.0;
    squeeze.command.max_effort = 50.0;  // Close gently
    
    ROS_INFO("Sending squeeze goal");
    gripper_client_->sendGoal(squeeze);
    gripper_client_->waitForResult();
    if(gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("The gripper closed!");
    else
      ROS_INFO("The gripper failed to close.");
  }
};

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_gripper");

  Gripper_Right gripper_right_;
  Gripper_Left gripper_left_;
  ROS_INFO("Move start");
  gripper_right_.open();
  gripper_left_.open();
  // gripper_right_.close();
  // gripper_left_.close();
  ros::NodeHandle nh;
  ROS_INFO("Get Model State");
  ros::ServiceClient getModelStateClient = nh.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
  getModelStateClient.waitForExistence();
  gazebo_msgs::GetModelState getRobotState;
  getRobotState.request.model_name = "pr2";
  ROS_INFO("robot location");
  if(getModelStateClient.call(getRobotState))
  {
    ROS_INFO("%f",getRobotState.response.pose.position.x);
    ROS_INFO("%f",getRobotState.response.pose.position.y);
    ROS_INFO("%f",getRobotState.response.pose.position.z);
  }
  ros::ServiceClient getLinkStateClient = nh.serviceClient<gazebo_msgs::GetLinkState>("/gazebo/get_link_state");
  gazebo_msgs::GetLinkState getGripperLinkState;
  getGripperLinkState.request.link_name = "r_wrist_roll_link";
  ROS_INFO("right gripper location");
  if(getLinkStateClient.call(getGripperLinkState))
  {
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.x);
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.y);
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.z);
  }
  getGripperLinkState.request.link_name = "l_wrist_roll_link";
  ROS_INFO("left gripper location");
  if(getLinkStateClient.call(getGripperLinkState))
  {
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.x);
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.y);
    ROS_INFO("%f",getGripperLinkState.response.link_state.pose.position.z);
  }

  getRobotState.request.model_name = "wood_cube_10cm";
  ROS_INFO("wooden cube right location");
  if(getModelStateClient.call(getRobotState))
  {
    ROS_INFO("%f",getRobotState.response.pose.position.x);
    ROS_INFO("%f",getRobotState.response.pose.position.y);
    ROS_INFO("%f",getRobotState.response.pose.position.z);
  }
  getRobotState.request.model_name = "wood_cube_10cm_clone";
  ROS_INFO("wooden cube left location");
  if(getModelStateClient.call(getRobotState))
  {
    ROS_INFO("%f",getRobotState.response.pose.position.x);
    ROS_INFO("%f",getRobotState.response.pose.position.y);
    ROS_INFO("%f",getRobotState.response.pose.position.z);
  }

//   tf::TransformListener listener;
//   listener.waitForTransform("/world", "/base_link", ros::Time(0), ros::Duration(3.0));

//   geometry_msgs::PointStamped point_world;
// geometry_msgs::PointStamped point_robot;

//   point_world.header.frame_id = "/world";
//   point_world.header.stamp = ros::Time(0);
//   point_world.point.x = 1/* Your world x coordinate */;
//   point_world.point.y = 1 Your world y coordinate ;
//   point_world.point.z = 1/* Your world z coordinate */;

//   try {
//       listener.transformPoint("/base_link", point_world, point_robot);
//       ROS_INFO("%f",point_robot.point.x);
//       ROS_INFO("%f",point_robot.point.y);
//       ROS_INFO("%f",point_robot.point.z);

//   } catch (tf::TransformException &ex) {
//       ROS_ERROR("Transform failed: %s", ex.what());
//   }
  return 0;
}
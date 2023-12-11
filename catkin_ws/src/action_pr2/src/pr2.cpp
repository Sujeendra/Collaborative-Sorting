#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ros/ros.h>
#include <pr2_controllers_msgs/JointTrajectoryAction.h>
#include <pr2_controllers_msgs/JointTrajectoryControllerState.h>
#include <actionlib/client/simple_action_client.h>
#include <moveit_msgs/GetPositionIK.h>
#include <moveit_msgs/GetPositionFK.h>
#include <action_pr2/ExecuteCartesianTrajectory.h>
#include <vector>
#include <iostream>
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/GetModelState.h>
#include <thread>
#include <chrono>
#include <cmath>
#include <pr2_controllers_msgs/Pr2GripperCommandAction.h>
#include <gazebo_msgs/GetLinkState.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>
#include <action_pr2/ExecuteCartesianTrajectory.h>

#define MAX_JOINT_VEL 0.5  //in radians/sec

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

class RobotDriver
{
private:
  //! The node handle we'll be using
  ros::NodeHandle nh_;
  //! We will be publishing to the "/base_controller/command" topic to issue commands
  ros::Publisher cmd_vel_pub_;

public:
  //! ROS node initialization
  RobotDriver(ros::NodeHandle &nh)
  {
    nh_ = nh;
    //set up the publisher for the cmd_vel topic
    cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/base_controller/command", 1);
  }
  //linear_x is forward and linear_y is backward linear_z is theta or turning in any direction
  bool drive()
  {
    std::cout << "Robot started moving";
    ros::ServiceClient getModelStateClient = nh_.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    getModelStateClient.waitForExistence();
    gazebo_msgs::GetModelState getRobotState;
    getRobotState.request.model_name = "pr2";
    int iter=0;
    while(getModelStateClient.call(getRobotState) && iter<140)
    {
      auto current_x=getRobotState.response.pose.position.x;
      auto current_y=getRobotState.response.pose.position.y;
      //we will be sending commands of type "twist"
      geometry_msgs::Twist base_cmd;
      double theta = std::abs(std::atan(current_x/current_y));
      base_cmd.angular.z = theta;
      base_cmd.linear.x = 1;
      cmd_vel_pub_.publish(base_cmd);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      // ROS_INFO("theta %f",theta);
      iter+=1;
    
    }
    auto current_x=getRobotState.response.pose.position.x;
    auto current_y=getRobotState.response.pose.position.y;

    while(getModelStateClient.call(getRobotState) && (current_x<-2.6 || current_y<-0.5))
    {
      current_x=getRobotState.response.pose.position.x;
      current_y=getRobotState.response.pose.position.y;
      geometry_msgs::Twist base_cmd;
      base_cmd.linear.x = 1;
      cmd_vel_pub_.publish(base_cmd);
    }
    iter=0;
    while(getModelStateClient.call(getRobotState) && iter<50)
    {
      current_x=getRobotState.response.pose.position.x;
      current_y=getRobotState.response.pose.position.y;
      geometry_msgs::Twist base_cmd;
      double theta = std::abs(std::atan(current_x/current_y));
      base_cmd.angular.z = -theta;
      base_cmd.linear.x = 1;
      cmd_vel_pub_.publish(base_cmd);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      iter+=1;
    }
    while(getModelStateClient.call(getRobotState) && (current_x<-0.2 || current_y<-0.2))
    {
      current_x=getRobotState.response.pose.position.x;
      current_y=getRobotState.response.pose.position.y;
      geometry_msgs::Twist base_cmd;
      base_cmd.linear.x = 0.2;
      cmd_vel_pub_.publish(base_cmd);
    }

    return true;
  }
  bool drive_backwards()
  {
    std::cout << "Robot started moving";
    ros::ServiceClient getModelStateClient = nh_.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    getModelStateClient.waitForExistence();
    gazebo_msgs::GetModelState getRobotState;
    getRobotState.request.model_name = "pr2";
    int iter=0;
    while(getModelStateClient.call(getRobotState) && iter<625)
    {
      auto current_x=getRobotState.response.pose.position.x;
      auto current_y=getRobotState.response.pose.position.y;
      //we will be sending commands of type "twist"
      geometry_msgs::Twist base_cmd;
      double theta = std::abs(std::atan(current_x/current_y));
      base_cmd.angular.z = -theta;
      base_cmd.linear.x = 1;
      cmd_vel_pub_.publish(base_cmd);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      // ROS_INFO("theta %f",theta);
      iter+=1;
    
    }

    return true;
  }
  bool move_back()
  {
    std::cout << "Robot started moving";
    int iter=0;
    while(iter<60)
    {
      geometry_msgs::Twist base_cmd;
      base_cmd.linear.x = -1;
      cmd_vel_pub_.publish(base_cmd);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      iter+=1;
    }

    return true;
  }

};
bool call_execute_cartesian_ik_trajectory_right(
    const std::string& frame,
    const std::vector<std::vector<double>>& positions,
    const std::vector<std::vector<double>>& orientations,
    ros::ServiceClient& client)
{
    action_pr2::ExecuteCartesianTrajectory srv;
    srv.request.header.frame_id = frame;
    srv.request.header.stamp = ros::Time::now();

    // Fill in the poses
    for (size_t i = 0; i < positions.size(); ++i)
    {
        geometry_msgs::Pose pose;
        pose.position.x = positions[i][0];
        pose.position.y = positions[i][1];
        pose.position.z = positions[i][2];
        pose.orientation.x = orientations[i][0];
        pose.orientation.y = orientations[i][1];
        pose.orientation.z = orientations[i][2];
        pose.orientation.w = orientations[i][3];
        srv.request.poses.push_back(pose);
    }

    // Call the service to execute the trajectory
    ROS_INFO("Calling execute_cartesian_ik_trajectory_right");
    if (client.call(srv))
    {
        return srv.response.success;
    }
    else
    {
        ROS_ERROR("Error when calling execute_cartesian_ik_trajectory_right");
        return false;
    }
}

int main(int argc, char** argv)
{

  ros::init(argc, argv, "action_pr2");
  ros::NodeHandle nh;
  Gripper_Right gripper_right_;
  Gripper_Left gripper_left_;

  //(1)
  ROS_INFO("Gripper started");
  gripper_right_.open();
  gripper_left_.open();


  //(2)
  ros::ServiceClient executeCartesianIKClientRight = nh.serviceClient<action_pr2::ExecuteCartesianTrajectory>("execute_cartesian_ik_trajectory_right");
  ros::ServiceClient executeCartesianIKClientLeft = nh.serviceClient<action_pr2::ExecuteCartesianTrajectory>("execute_cartesian_ik_trajectory_left");
    try
    {

        std::vector<std::vector<double>> positions_r = {{0.73, -0.19, 0.74}};
        std::vector<std::vector<double>> orientations_r = {{0.92, -0.09, 0.0, 1.0}};

        std::vector<std::vector<double>> positions_l = {{0.73, 0.19, 0.74}};
        std::vector<std::vector<double>> orientations_l = {{0.92, -0.09, 0.0, 1.0}};

        bool success_r = call_execute_cartesian_ik_trajectory_right("/base_link", positions_r, orientations_r, executeCartesianIKClientRight);

        bool success_l = call_execute_cartesian_ik_trajectory_right("/base_link", positions_l, orientations_l, executeCartesianIKClientLeft);

        if (success_r && success_l)
        {
            ROS_INFO("Trajectory succeeded!");
        }
        else
        {
            ROS_INFO("Trajectory failed.");
        }
    }
    catch (const std::exception& e)
    {
        ROS_ERROR("Service call failed: %s", e.what());
        return 1;
    }

    //(3)
    RobotDriver driver(nh);
    driver.drive();

    //(4)
    try
    {

        std::vector<std::vector<double>> positions_r = {{0.73, -0.19, 0.74}};
        std::vector<std::vector<double>> orientations_r = {{0.92, -0.09, 0.0, 1.0}};

        std::vector<std::vector<double>> positions_l = {{0.73, 0.19, 0.74}};
        std::vector<std::vector<double>> orientations_l = {{0.92, -0.09, 0.0, 1.0}};

        bool success_r = call_execute_cartesian_ik_trajectory_right("/base_link", positions_r, orientations_r, executeCartesianIKClientRight);

        bool success_l = call_execute_cartesian_ik_trajectory_right("/base_link", positions_l, orientations_l, executeCartesianIKClientLeft);

        if (success_r && success_l)
        {
            ROS_INFO("Trajectory succeeded!");
        }
        else
        {
            ROS_INFO("Trajectory failed.");
        }
    }
    catch (const std::exception& e)
    {
        ROS_ERROR("Service call failed: %s", e.what());
        return 1;
    }
    //(5)
    gripper_right_.close();
    gripper_left_.close();

    //(6)
    try
    {

        std::vector<std::vector<double>> positions_r = {{0.73, -0.19, 0.83}};
        std::vector<std::vector<double>> orientations_r = {{0.92, -0.09, 0.0, 1.0}};

        std::vector<std::vector<double>> positions_l = {{0.73, 0.19, 0.83}};
        std::vector<std::vector<double>> orientations_l = {{0.92, -0.09, 0.0, 1.0}};

        bool success_r = call_execute_cartesian_ik_trajectory_right("/base_link", positions_r, orientations_r, executeCartesianIKClientRight);

        bool success_l = call_execute_cartesian_ik_trajectory_right("/base_link", positions_l, orientations_l, executeCartesianIKClientLeft);

        if (success_r && success_l)
        {
            ROS_INFO("Trajectory succeeded!");
        }
        else
        {
            ROS_INFO("Trajectory failed.");
        }
    }
    catch (const std::exception& e)
    {
        ROS_ERROR("Service call failed: %s", e.what());
        return 1;
    }

    //(8)
    driver.drive_backwards();
  //(9)
      try
      {

          std::vector<std::vector<double>> positions_r = {{0.73, -0.19, 0.70}};
          std::vector<std::vector<double>> orientations_r = {{0.92, -0.09, 0.0, 1.0}};

          std::vector<std::vector<double>> positions_l = {{0.73, 0.19, 0.70}};
          std::vector<std::vector<double>> orientations_l = {{0.92, -0.09, 0.0, 1.0}};

          bool success_r = call_execute_cartesian_ik_trajectory_right("/base_link", positions_r, orientations_r, executeCartesianIKClientRight);

          bool success_l = call_execute_cartesian_ik_trajectory_right("/base_link", positions_l, orientations_l, executeCartesianIKClientLeft);

          if (success_r && success_l)
          {
              ROS_INFO("Trajectory succeeded!");
          }
          else
          {
              ROS_INFO("Trajectory failed.");
          }
      }
      catch (const std::exception& e)
      {
          ROS_ERROR("Service call failed: %s", e.what());
          return 1;
      }
    //(10)
    gripper_right_.open();
    gripper_left_.open();

    //(11)
    driver.move_back();

  return 0;
}

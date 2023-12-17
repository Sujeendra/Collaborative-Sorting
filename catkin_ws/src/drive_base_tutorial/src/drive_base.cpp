#include <iostream>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <gazebo_msgs/GetModelState.h>
#include <thread>
#include <chrono>
#include <cmath>

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

  //! Loop forever while sending drive commands based on keyboard input
  // bool driveKeyboard()
  // {
  //   std::cout << "Type a command and then press enter.  "
  //     "Use '+' to move forward, 'l' to turn left, "
  //     "'r' to turn right, '.' to exit.\n";

  //   //we will be sending commands of type "twist"
  //   geometry_msgs::Twist base_cmd;

  //   char cmd[50];
  //   while(nh_.ok()){

  //     std::cin.getline(cmd, 50);
  //     if(cmd[0]!='+' && cmd[0]!='l' && cmd[0]!='r' && cmd[0]!='.')
  //     {
  //       std::cout << "unknown command:" << cmd << "\n";
  //       continue;
  //     }

  //     base_cmd.linear.x = base_cmd.linear.y = base_cmd.angular.z = 0;
  //     //move forward
  //     if(cmd[0]=='+'){
  //       base_cmd.linear.x = 0.25;
  //     }
  //     //turn left (yaw) and drive forward at the same time
  //     else if(cmd[0]=='l'){
  //       base_cmd.angular.z = 0.75;
  //       base_cmd.linear.x = 0.25;
  //     }
  //     //turn right (yaw) and drive forward at the same time
  //     else if(cmd[0]=='r'){
  //       base_cmd.angular.z = -0.75;
  //       base_cmd.linear.x = 0.25;
  //     }
  //     //quit
  //     else if(cmd[0]=='.'){
  //       break;
  //     }

  //     //publish the assembled command
  //     cmd_vel_pub_.publish(base_cmd);
  //   }
  //   return true;
  //}
  //linear_x is forward and linear_y is backward linear_z is theta or turning in any direction
  bool drive()
  {
    std::cout << "Robot started moving";
    ros::ServiceClient getModelStateClient = nh_.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    getModelStateClient.waitForExistence();
    gazebo_msgs::GetModelState getRobotState;
    getRobotState.request.model_name = "pr2";
    int iter=0;
    while(getModelStateClient.call(getRobotState) && iter<3)
    {
      auto current_x=getRobotState.response.pose.position.x;
      auto current_y=getRobotState.response.pose.position.y;
      // ROS_INFO("x%f",getRobotState.response.pose.orientation.x);
      // ROS_INFO("y%f",getRobotState.response.pose.orientation.y);
      // ROS_INFO("z%f",getRobotState.response.pose.orientation.z);
      // ROS_INFO("w%f",getRobotState.response.pose.orientation.w);

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

};

int main(int argc, char** argv)
{
  //init the ROS node
  ros::init(argc, argv, "robot_driver");
  ros::NodeHandle nh;

  RobotDriver driver(nh);
  //driver.driveKeyboard();
  driver.drive();
}
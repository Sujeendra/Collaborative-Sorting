# http://ros.org/doc/groovy/api/catkin/html/user_guide/supposed.html
cmake_minimum_required(VERSION 2.8.3)
project(kinematics_msgs)

find_package(catkin REQUIRED COMPONENTS geometry_msgs trajectory_msgs arm_navigation_msgs message_generation)

add_message_Files(
  FILES KinematicSolverInfo.msg PositionIKRequest.msg
  DIRECTORY msg)
add_service_Files(
  FILES GetConstraintAwarePositionIK.srv 	GetKinematicSolverInfo.srv GetPositionFK.srv GetPositionIK.srv
  DIRECTORY srv)

generate_messages(
  DEPENDENCIES geometry_msgs trajectory_msgs arm_navigation_msgs
)

catkin_package(
    CATKIN_DEPENDS geometry_msgs trajectory_msgs arm_navigation_msgs message_runtime
)

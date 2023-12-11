#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>
#include <gazebo_msgs/GetModelState.h>
#include <ik_trajectory_tutorial/ExecuteCartesianIKTrajectory.h>

bool call_execute_cartesian_ik_trajectory_right(
    const std::string& frame,
    const std::vector<std::vector<double>>& positions,
    const std::vector<std::vector<double>>& orientations,
    ros::ServiceClient& client)
{
    ik_trajectory_tutorial::ExecuteCartesianIKTrajectory srv;
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
    ros::init(argc, argv, "test_cartesian_ik_trajectory_executer");
    ros::NodeHandle nh;
    tf::TransformListener tfListener;
    ros::Duration(0.5).sleep(); // Give the transform listener time to get some frames

    ros::ServiceClient getModelStateClient = nh.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    ros::ServiceClient executeCartesianIKClientRight = nh.serviceClient<ik_trajectory_tutorial::ExecuteCartesianIKTrajectory>("execute_cartesian_ik_trajectory_right");
    ros::ServiceClient executeCartesianIKClientLeft = nh.serviceClient<ik_trajectory_tutorial::ExecuteCartesianIKTrajectory>("execute_cartesian_ik_trajectory_left");

    ros::service::waitForService("/gazebo/get_model_state");

    try
    {
        gazebo_msgs::GetModelState srv;
        srv.request.model_name = "wood_cube_10cm";
        srv.request.relative_entity_name = "world";
        getModelStateClient.call(srv);
        double x_r = srv.response.pose.position.x;
        double y_r = srv.response.pose.position.y;

        srv.request.model_name = "wood_cube_10cm_clone";
        getModelStateClient.call(srv);
        double x_l = srv.response.pose.position.x;
        double y_l = srv.response.pose.position.y;

        srv.request.model_name = "pr2";
        getModelStateClient.call(srv);
        double r_x = srv.response.pose.position.x;
        double r_y = srv.response.pose.position.y;

        std::vector<std::vector<double>> positions_r = {{x_r - r_x - 0.2, y_r - r_y - 0.1, 0.66}};
        std::vector<std::vector<double>> orientations_r = {{0.02, -0.09, 0.0, 1.0}};

        std::vector<std::vector<double>> positions_l = {{x_l - r_x - 0.2, y_l - r_y - 0.1, 0.66}};
        std::vector<std::vector<double>> orientations_l = {{0.02, -0.09, 0.0, 1.0}};

        bool success_r = call_execute_cartesian_ik_trajectory_right("/base_link", positions_r, orientations_r, executeCartesianIKClientRight);

        bool success_l = call_execute_cartesian_ik_trajectory_right("/base_link", positions_l, orientations_l, executeCartesianIKClientLeft);

        tf::StampedTransform transform_r;
        tfListener.lookupTransform("r_wrist_roll_link", "base_link", ros::Time(0), transform_r);
        tf::StampedTransform transform_l;
        tfListener.lookupTransform("l_wrist_roll_link", "base_link", ros::Time(0), transform_l);

        ROS_INFO("End Cartesian pose right: trans (%2.3f %2.3f %2.3f) rot (%2.3f %2.3f %2.3f %2.3f)",
                 transform_r.getOrigin().x(), transform_r.getOrigin().y(), transform_r.getOrigin().z(),
                 transform_r.getRotation().x(), transform_r.getRotation().y(), transform_r.getRotation().z(), transform_r.getRotation().w());

        ROS_INFO("End Cartesian pose left: trans (%2.3f %2.3f %2.3f) rot (%2.3f %2.3f %2.3f %2.3f)",
                 transform_l.getOrigin().x(), transform_l.getOrigin().y(), transform_l.getOrigin().z(),
                 transform_l.getRotation().x(), transform_l.getRotation().y(), transform_l.getRotation().z(), transform_l.getRotation().w());

        if (success_r && success_l)
        {
            ROS_INFO("Trajectory succeeded!");
        }
        else
        {
            ROS_INFO("Trajectory failed.");
        }
    }
    catch (ros::ServiceException& e)
    {
        ROS_ERROR("Service call failed: %s", e.what());
        return 1;
    }

    return 0;
}

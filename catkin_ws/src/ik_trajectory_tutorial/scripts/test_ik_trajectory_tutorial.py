#test client for ik_trajectory_tutorial

import roslib
roslib.load_manifest('ik_trajectory_tutorial')
import rospy
from ik_trajectory_tutorial.srv import ExecuteCartesianIKTrajectory
from geometry_msgs.msg import Pose
import time
import sys
import pdb
import tf
from gazebo_msgs.srv import GetModelState

#execute a Cartesian trajectory defined by a root frame, a list of 
#positions (x,y,z), and a list of orientations (quaternions: x,y,z,w)
def call_execute_cartesian_ik_trajectory_right(frame, positions, orientations):
    rospy.wait_for_service("execute_cartesian_ik_trajectory_right")

    #fill in the header (don't need seq)
    header = rospy.Header()
    header.frame_id = frame
    header.stamp = rospy.get_rostime()

    #fill in the poses
    poses = []
    for (position, orientation) in zip(positions, orientations):
        pose = Pose()
        pose.position.x = position[0]
        pose.position.y = position[1]
        pose.position.z = position[2]
        pose.orientation.x = orientation[0]
        pose.orientation.y = orientation[1]
        pose.orientation.z = orientation[2]
        pose.orientation.w = orientation[3]
        poses.append(pose)

    #call the service to execute the trajectory
    print ("calling execute_cartesian_ik_trajectory_right")
    try:
        s = rospy.ServiceProxy("execute_cartesian_ik_trajectory_right", \
                                   ExecuteCartesianIKTrajectory)
        resp = s(header, poses)
    except rospy.ServiceException as e:
        print ("error when calling execute_cartesian_ik_trajectory_right: %s"%e)
        return 0
    return resp.success

def call_execute_cartesian_ik_trajectory_left(frame, positions, orientations):
    rospy.wait_for_service("execute_cartesian_ik_trajectory_left")

    #fill in the header (don't need seq)
    header = rospy.Header()
    header.frame_id = frame
    header.stamp = rospy.get_rostime()

    #fill in the poses
    poses = []
    for (position, orientation) in zip(positions, orientations):
        pose = Pose()
        pose.position.x = position[0]
        pose.position.y = position[1]
        pose.position.z = position[2]
        pose.orientation.x = orientation[0]
        pose.orientation.y = orientation[1]
        pose.orientation.z = orientation[2]
        pose.orientation.w = orientation[3]
        poses.append(pose)

    #call the service to execute the trajectory
    print ("calling execute_cartesian_ik_trajectory_left")
    try:
        s = rospy.ServiceProxy("execute_cartesian_ik_trajectory_left", \
                                   ExecuteCartesianIKTrajectory)
        resp = s(header, poses)
    except rospy.ServiceException as e:
        print ("error when calling execute_cartesian_ik_trajectory_left: %s"%e)
        return 0
    return resp.success

#pretty-print list to string
def pplist(list):
    return ' '.join(['%2.3f'%x for x in list])

#print out the positions, velocities, and efforts of the right arm joints
if __name__ == "__main__":
    rospy.init_node("test_cartesian_ik_trajectory_executer")
    tf_listener = tf.TransformListener()
    time.sleep(.5) #give the transform listener time to get some frames

    rospy.wait_for_service('/gazebo/get_model_state')
    try:


        positions_r = [[.76, -.19, .75]]
        orientations_r = [[.92, -.09, 0.0, 1.0]]

        positions_l = [[.76, .19, .75]]
        orientations_l = [[.92, -.09, 0.0, 1.0]]

        # positions_l = [[.76, .19, .83], [0.59, 0.36, 0.93]]
        # orientations_l = [[.02, -.09, 0.0, 1.0], [0.65, -0.21, .38, .62]]
        success_r = call_execute_cartesian_ik_trajectory_right("/base_link", \
                positions_r, orientations_r)


        success_l = call_execute_cartesian_ik_trajectory_left("/base_link", \
                positions_l, orientations_l)

        #check the final pose
        (trans_r, rot_r) = tf_listener.lookupTransform('r_wrist_roll_link', 'base_link', rospy.Time(0))
        transformation_matrix = tf_listener.fromTranslationRotation(trans_r, rot_r)
        print(transformation_matrix)
        (trans_l, rot_l) = tf_listener.lookupTransform('base_link', 'l_wrist_roll_link', rospy.Time(0))
        print ("end Cartesian pose: trans", pplist(trans_r), "rot", pplist(rot_r))
        print ("end Cartesian pose: trans", pplist(trans_l), "rot", pplist(rot_l))

        if success_r and success_l:
            print ("trajectory succeeded!")
        else:
            print ("trajectory failed.")


    except rospy.ServiceException as e:
        rospy.logerr(f"Service call failed: {e}")

    # positions_r = [[.76, -.19, .83], [0.59, -0.36, 0.93]]
    # orientations_r = [[.02, -.09, 0.0, 1.0], [0.65, -0.21, .38, .62]]
    

    
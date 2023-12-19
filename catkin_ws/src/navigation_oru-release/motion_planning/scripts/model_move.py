#!/usr/bin/env python
import rospy
from gazebo_msgs.srv import GetModelState, SetModelState
from gazebo_msgs.msg import ModelState
from gazebo_ros_link_attacher.srv import Attach, AttachRequest, AttachResponse
from gazebo_ros_link_attacher.srv import SetStatic, SetStaticRequest, SetStaticResponse

class ObjectMover:
    def __init__(self):
        rospy.init_node('object_mover', anonymous=True)

        # Publisher to set the model state of the object
        self.object_state_publisher = rospy.Publisher('/gazebo/set_model_state', ModelState, queue_size=10)
        self.setstatic_srv = rospy.ServiceProxy("/link_attacher_node/setstatic", SetStatic)
        self.attach_srv = rospy.ServiceProxy("/link_attacher_node/attach", Attach)
        self.setstatic_srv.wait_for_service()
        self.attach_srv.wait_for_service()

        # Initial object state
        self.object_state = ModelState()
        self.object_state.model_name = 'X1-Y1-Z2_1'  # Replace with the actual model name
        self.object_state.pose.position.x = initial_x_position  # Replace with the initial x position
        self.object_state.pose.position.y = initial_y_position  # Replace with the initial y position
        self.object_state.pose.position.z = initial_z_position  # Replace with the initial z position

    def move_object_with_table(self):
        # Get the current table state       
        table_model_state = self.get_model_state('table_model')  # Replace with the actual table model name
        if table_model_state is not None and table_model_state.pose is not None:
            rospy.loginfo(f"Table Pose: {table_model_state.pose}")

            # Update the object's position based on the table's position
            self.object_state.pose.position.x = table_model_state.pose.position.x + offset_x
            self.object_state.pose.position.y = table_model_state.pose.position.y + offset_y
            self.object_state.pose.position.z = table_model_state.pose.position.z + offset_z

            rospy.loginfo(f"Object New Pose: {self.object_state.pose}")

            # Publish the updated object state
            self.object_state_publisher.publish(self.object_state)
            req = AttachRequest()
            req.model_name_1 = 'X1-Y1-Z2_1'
            req.link_name_1 = "link"
            req.model_name_2 = "table_model"
            req.link_name_2 = "base_link"
            self.attach_srv.call(req)

            req = SetStaticRequest()
            print("{} TO HOME".format('X1-Y1-Z2_1'))
            req.model_name = 'X1-Y1-Z2_1'
            req.link_name = "link"
            req.set_static = True

            self.setstatic_srv.call(req)

    def get_model_state(self, model_name):
        # Service client to get the model state
        try:
            get_model_state = rospy.ServiceProxy('/gazebo/get_model_state', GetModelState)
            response = get_model_state(model_name, 'world')
            return response  # Return the entire response
        except rospy.ServiceException as e:
            rospy.logerr(f"Service call failed: {e}")
            return None

    def run(self):
        rate = rospy.Rate(10)  # 10 Hz, adjust as needed

        while not rospy.is_shutdown():
            pick_and_place_switch = rospy.get_param("/pick_and_place_done")
            if(pick_and_place_switch):
                self.move_object_with_table()
                rospy.set_param("/pick_and_place_done",True)
            rate.sleep()

if __name__ == '__main__':


    # Define initial object position
    initial_x_position = 0.0
    initial_y_position = 0.0
    initial_z_position = 0.0

    # Define the offset from the table's position
    offset_x = 0.1
    offset_y = 0.0
    offset_z = 0.22

    object_mover = ObjectMover()
    object_mover.run()

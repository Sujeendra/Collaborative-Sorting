
(cl:in-package :asdf)

(defsystem "action_pr2-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "ExecuteCartesianTrajectory" :depends-on ("_package_ExecuteCartesianTrajectory"))
    (:file "_package_ExecuteCartesianTrajectory" :depends-on ("_package"))
  ))
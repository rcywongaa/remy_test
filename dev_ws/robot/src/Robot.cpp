#include "Robot.hpp"

#include <array>

Robot::Robot(const robot_model_loader::RobotModelLoader& model_loader)
{
  // Set up MoveIt kinematic model
  kinematic_model = model_loader.getModel();
  current_state = std::make_shared<moveit::core::RobotState>(kinematic_model);
}

bool Robot::run(robot::msg::PointTrajectory traj)
{
  const moveit::core::JointModelGroup* joint_model_group = kinematic_model->getJointModelGroup("manipulator");
  const Eigen::Isometry3d& end_effector_state = current_state->getGlobalLinkTransform("ee");
  double timeout = 0.1;
  bool is_found_ik = current_state->setFromIK(joint_model_group, end_effector_state, timeout);
  if (is_found_ik)
  {
    // execute IK
    ;
  }
  return is_found_ik;
}

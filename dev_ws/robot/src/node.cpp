#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "robot_msgs/msg/point_trajectory.hpp"
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <rclcpp/qos.hpp>

#include "Robot.hpp"

class RobotNode : public rclcpp::Node
{
  public:
    RobotNode() : rclcpp::Node("RobotNode")
    {
      RCLCPP_INFO(this->get_logger(), "RobotNode created!");
    }

    void init()
    {
      RCLCPP_INFO(this->get_logger(), "RobotNode initialized!");
      robot_model_loader::RobotModelLoader model_loader(shared_from_this(), std::string("robot_description"));
      joint_traj_pub = this->create_publisher<trajectory_msgs::msg::JointTrajectory>(
          declare_and_get_parameter("joint_trajectory_topic").as_string(),
          1);
      point_traj_sub = this->create_subscription<robot_msgs::msg::PointTrajectory>(
          declare_and_get_parameter("ee_trajectory_topic").as_string(),
          rclcpp::QoS(1).transient_local(),
          [&](const robot_msgs::msg::PointTrajectory::SharedPtr msg)
          {
            RCLCPP_INFO(this->get_logger(), "Received trajectory!");
            std::optional<trajectory_msgs::msg::JointTrajectory> joint_trajectory =
                robot->create_joint_trajectory(*msg);
            if (joint_trajectory)
            {
              joint_traj_pub->publish(*joint_trajectory);
            }
            else
            {
              RCLCPP_ERROR(this->get_logger(), "Failed to generate trajectory!");
            }
          });
      robot = std::make_unique<Robot>(model_loader.getModel());
    }

  private:
    rclcpp::Parameter declare_and_get_parameter(std::string param_name)
    {
      this->declare_parameter(param_name);
      return this->get_parameter(param_name);
    }
    std::unique_ptr<Robot> robot;

    rclcpp::Subscription<robot_msgs::msg::PointTrajectory>::SharedPtr point_traj_sub;
    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr joint_traj_pub;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  std::shared_ptr<RobotNode> node = std::make_shared<RobotNode>();
  node->init();
  rclcpp::spin(node);
  rclcpp::shutdown();
}

#include <bot_control/velocity_controller.hpp>
#include <Eigen/Geometry>

using std::placeholders::_1;

SimpleController::SimpleController(const std::string &name) : Node(name)
{
    // Initialize parameters
    this->declare_parameter<double>("wheel_radius", 0.5);
    this->declare_parameter<double>("wheel_separation", 0.306);

    wheel_radius_ = this->get_parameter("wheel_radius").as_double();
    wheel_separation_ = this->get_parameter("wheel_separation").as_double();

    RCLCPP_INFO(this->get_logger(), "Wheel radius: %f", wheel_radius_);
    RCLCPP_INFO(this->get_logger(), "Wheel separation: %f", wheel_separation_);
    
    // Create subscriber and publisher
    wheel_cmd_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("/velocity_controller/commands", 10);
    
    vel_sub_ = this->create_subscription<geometry_msgs::msg::TwistStamped>(
        "/bot_control/cmd_vel", 10, std::bind(&SimpleController::velCallback, this, _1));
    
    speed_conversion_ << wheel_radius_/2, wheel_radius_/2,
                         wheel_radius_ / wheel_separation_, -wheel_radius_ / wheel_separation_;

    RCLCPP_INFO(this->get_logger(), "Eigen speed conversion matrix:\n%f %f\n%f %f", 
                speed_conversion_(0,0), speed_conversion_(0,1), 
                speed_conversion_(1,0), speed_conversion_(1,1));
}

void SimpleController::velCallback(const geometry_msgs::msg::TwistStamped & msg) {
    Eigen::Vector2d robot_speed(msg.twist.linear.x, msg.twist.angular.z);
    Eigen::Vector2d wheel_speed = speed_conversion_.inverse() * robot_speed;

    std_msgs::msg::Float64MultiArray wheel_cmd_msg;
    wheel_cmd_msg.data.push_back(wheel_speed(1));
    wheel_cmd_msg.data.push_back(wheel_speed(0));

    wheel_cmd_pub_ ->publish(wheel_cmd_msg);
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleController>("velocity_controller");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
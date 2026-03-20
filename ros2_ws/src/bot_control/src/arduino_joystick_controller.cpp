#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include <libserial/SerialPort.h>
#include <iostream>

using namespace LibSerial;

class ArduinoJoystick : public rclcpp::Node
{
public:
    ArduinoJoystick() : Node("arduino_joystick_controller")
    {
        publisher_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
                    "/bot_control/cmd_vel", 10);

        try
        {
            serial_port_.Open("/dev/ttyUSB0");
            serial_port_.SetBaudRate(BaudRate::BAUD_115200);
        }
        catch(...)
        {
            RCLCPP_ERROR(this->get_logger(),"Failed to open serial port");
        }

        this->declare_parameter<double>("linear_gain", 2.0);
        this->declare_parameter<double>("angular_gain", 2.0);

        linear_gain_ = this->get_parameter("linear_gain").as_double();
        angular_gain_ = this->get_parameter("angular_gain").as_double();

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(50),
            std::bind(&ArduinoJoystick::read_serial,this));
    }

private:
    double linear_gain_;
    double angular_gain_;
    void read_serial()
    {
        std::string line;

        try
        {
            serial_port_.ReadLine(line);

            int x,y;
            sscanf(line.c_str(), "%d,%d",&x,&y);

            geometry_msgs::msg::TwistStamped msg;

            msg.header.stamp = this->get_clock()->now();
            msg.header.frame_id = "base_footprint";
            
            double linear = (x - 512) / 512.0;
            double angular = -(y - 512) / 512.0;

            if (fabs(linear) < 0.05) linear = 0.0;
            if (fabs(angular) < 0.05) angular = 0.0;

            msg.twist.linear.x = linear_gain_ * linear;
            msg.twist.angular.z = angular_gain_ * angular;

            publisher_->publish(msg);

        }
        catch(...)
        {
        }
    }

    SerialPort serial_port_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc,char **argv)
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<ArduinoJoystick>());
    rclcpp::shutdown();
    return 0;
}

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    wheel_radius_arg = DeclareLaunchArgument(
        'wheel_radius',
        default_value='0.033',
        description='Radius of the robot wheels'
    )
    wheel_separation_arg = DeclareLaunchArgument(
        'wheel_separation',
        default_value='0.14',
        description='Distance between the robot wheels'
    )

    wheel_radius = LaunchConfiguration('wheel_radius')
    wheel_separation = LaunchConfiguration('wheel_separation')

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager"
        ],
    )

    simple_controller = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "velocity_controller",
            "--controller-manager",
            "/controller_manager"
        ],
    )
    
    simple_controller_cpp = Node(
        package="bot_control",
        executable="velocity_controller",
        parameters=[{"wheel_radius": wheel_radius,
                    "wheel_separation": wheel_separation}],
    )

    arduino_joystick_controller = Node(
        package="bot_control",
        executable="arduino_joystick_controller",
        parameters=[{"linear_gain": 1.0, "angular_gain": 1.0}]
    )

    return LaunchDescription([
        wheel_radius_arg,
        wheel_separation_arg,   
        joint_state_broadcaster_spawner,
        simple_controller,
        simple_controller_cpp,
        arduino_joystick_controller
    ])
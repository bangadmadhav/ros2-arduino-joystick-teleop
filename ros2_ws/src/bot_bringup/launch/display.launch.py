from launch import LaunchDescription
from pathlib import Path

from launch.actions import IncludeLaunchDescription, SetEnvironmentVariable
from launch.substitutions import Command
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue

from ament_index_python.packages import get_package_share_directory

import os

def generate_launch_description():
    #Define package shares
    description_pkg_share = get_package_share_directory('bot_description')
    bringup_pkg_share = get_package_share_directory('bot_bringup')

    #Define Various Paths
    urdf_file = os.path.join(description_pkg_share, 'urdf', 'bot_description.urdf.xacro')
    rviz_config_file = os.path.join(bringup_pkg_share, 'config', 'displayTest.rviz')

    #Declare robot_description for robot_state_publisher node
    robot_model_description = ParameterValue(
        Command(['xacro ', urdf_file]),
        value_type=str
    )
    
    gazebo_resource_path = SetEnvironmentVariable(
        name="GZ_SIM_RESOURCE_PATH",
        value=[
            str(Path(description_pkg_share).parent.resolve())
        ]
    )

    #robot_state_publisher node
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[
            {"robot_description": robot_model_description},
            {"use_sim_time": True}
        ]
    )
    
    #rviz2 node to visualize the robot in rviz (ONLY FOR TESTING)
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config_file]
    )

    gazebo_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("ros_gz_sim"),
                "launch",
                "gz_sim.launch.py"
            )
        ),
        launch_arguments={"gz_args": "empty.sdf -r"}.items()
    )

    spawn_robot_node = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=["-topic", "robot_description"]
    )

    bridge_node = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        parameters=[{
            "config_file": os.path.join(bringup_pkg_share, "config", "gazebo_bridge.yaml")
        }],
        output="screen"
    )

    # 4. Return the LaunchDescription
    return LaunchDescription([
        robot_state_publisher_node,
        rviz_node,
        gazebo_resource_path,
        gazebo_node,
        spawn_robot_node,
        bridge_node
    ])
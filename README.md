# ROS2 Arduino Joystick Teleop

A ROS2 package for teleoperation of a differential drive robot in Gazebo simulation using a physical Arduino-based joystick as the input device.

---

## 🚀 Overview

This project bridges a physical Arduino joystick to a Gazebo-simulated differential drive robot through two custom ROS2 nodes. The joystick analog values are read over serial, converted to velocity commands, and then transformed into individual wheel speeds using differential drive inverse kinematics — fed into a `ros2_control` velocity controller.

```
[Arduino Joystick] → UART → [arduino_joystick_controller] → /bot_control/cmd_vel
    → [velocity_controller] → /velocity_controller/commands → [ros2_control] → [Gazebo]
```

---

## 🎥 Demo

[![Watch the demo](https://img.youtube.com/vi/KkGPCAaPrQs/0.jpg)](https://youtu.be/KkGPCAaPrQs)

---

## 🧩 Features

- Custom serial bridge reading Arduino analog joystick over UART at 115200 baud
- Configurable linear and angular gain via ROS2 parameters
- Analog deadzone filtering to prevent drift at center position
- Differential drive inverse kinematics using Eigen matrix inversion for wheel speed computation
- Publishes `TwistStamped` with proper `base_footprint` frame id
- Integrates with `ros2_control` `JointGroupVelocityController`
- Gazebo simulation via `ros_gz_sim` and `ros_gz_bridge`

---

## 🧠 How It Works

### `arduino_joystick_controller` node
Reads raw analog X/Y values (0–1023) from Arduino over serial, normalizes to `[-1, 1]`, applies deadzone filtering, scales by configurable gain, and publishes as `geometry_msgs/TwistStamped` on `/bot_control/cmd_vel`.

| Parameter | Default | Description |
|---|---|---|
| `linear_gain` | 1.0 | Scales normalized joystick axis to linear velocity |
| `angular_gain` | 1.0 | Scales normalized joystick axis to angular velocity |

### `velocity_controller` node
Subscribes to `/bot_control/cmd_vel` and converts `(v, ω)` to individual left/right wheel speeds using differential drive inverse kinematics via Eigen:

```
[v]   [r/2   r/2] [ω_left]
[ω] = [r/d   -r/d] [ω_right]

→ [ω_left, ω_right] = M⁻¹ · [v, ω]
```

Publishes to `/velocity_controller/commands` consumed by `ros2_control`.

| Parameter | Default | Description |
|---|---|---|
| `wheel_radius` | 0.033 | Wheel radius in metres |
| `wheel_separation` | 0.14 | Distance between wheels in metres |

---

## 📁 Repository Structure

```
ros2-joystick-teleop/
├── assets
└── ros2_ws/
    └── src/
        ├── bot_control/                        ← Custom nodes (primary contribution)
        │   ├── src/
        │   │   ├── arduino_joystick_controller.cpp  ← Serial bridge → TwistStamped
        │   │   └── velocity_controller.cpp          ← Diff drive kinematics → wheel cmds
        │   ├── include/bot_control/
        │   │   └── velocity_controller.hpp
        │   ├── CMakeLists.txt
        │   └── package.xml
        ├── bot_bringup/                        ← Launch and config files
        │   ├── launch/
        │   │   ├── controller.launch.py        ← Spawns controllers + both custom nodes
        │   │   └── display.launch.py           ← Gazebo + RViz2 + robot state publisher
        │   ├── config/
        │   │   ├── controller.yaml             ← ros2_control: JointGroupVelocityController
        │   │   ├── gazebo_bridge.yaml          ← ros_gz_bridge topic mappings
        │   │   └── displayTest.rviz            ← RViz2 config
        │   ├── CMakeLists.txt
        │   └── package.xml
        └── bot_description/                   ← Robot URDF/xacro and meshes
            ├── urdf/
            │   ├── bot_description.urdf.xacro
            │   ├── gazebo_setup.xacro
            │   ├── properties.xacro
            │   └── ros2_control_setup.xacro
            ├── meshes/                        ← STL files for visualization
            ├── CMakeLists.txt
            └── package.xml
```

---

## ⚙️ Tech Stack

`ROS2` · `C++` · `Python` · `Arduino` · `Gazebo` ·

---

## 🚀 Usage

**Terminal 1 — Launch simulation:**
```bash
ros2 launch bot_bringup display.launch.py
```

**Terminal 2 — Launch controllers and joystick node:**
```bash
ros2 launch bot_bringup controller.launch.py
```

Plug in the Arduino joystick over USB before launching Terminal 2.

---

## 👨‍💻 My Contribution

- Wrote `arduino_joystick_controller` node — serial bridge with deadzone filtering and configurable gain parameters
- Wrote `velocity_controller` node — differential drive inverse kinematics using Eigen matrix inversion
- Wrote both launch files integrating `ros2_control`, `ros_gz_bridge`, Gazebo, and RViz2
- Configured `ros2_control` with `JointGroupVelocityController` via `controller.yaml`
- Integrated the full pipeline from physical joystick input to simulated robot motion

> *Robot mesh files is not designed by me rather used only for reference.*

---

## 🎯 Key Takeaway

Demonstrates bridging physical hardware input into a ROS2 simulation pipeline, implementing differential drive kinematics as a ROS2 node, and integrating `ros2_control` with Gazebo via the `ros_gz_bridge`.

---

## 📸 System Architecture

<img src="/assets/rqt_graph.png" alt="RQT Graph">
*Node and topic connections*

<img src="/assets/tf2_tree.png" alt="TF2 Tree">
*Transform frame hierarchy*

<img src="/assets/gazebo_sim.png" alt="Gazebo Simulation">
*Differential drive robot in Gazebo*

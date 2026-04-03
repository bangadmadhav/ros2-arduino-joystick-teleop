# ROS2 Arduino Joystick Teleop

A ROS2 package for teleoperation of a differential drive robot in Gazebo simulation using a physical Arduino-based joystick as the input device.

---

## Overview

This project bridges a physical Arduino joystick to a Gazebo-simulated differential drive robot through two custom ROS2 nodes. The joystick analog values are read over serial, converted to velocity commands, and then transformed into individual wheel speeds using differential drive inverse kinematics — fed into a `ros2_control` velocity controller.

```
[Arduino Joystick] → UART → [arduino_joystick_controller] → /bot_control/cmd_vel
    → [velocity_controller] → /velocity_controller/commands → [ros2_control] → [Gazebo]
```

### Demo

Link to Video Demo: https://youtu.be/KkGPCAaPrQs

---

## Features

- Custom serial bridge reading Arduino analog joystick over UART at 115200 baud
- Configurable linear and angular gain via ROS2 parameters
- Analog deadzone filtering to prevent drift at center position
- Differential drive inverse kinematics using Eigen matrix inversion for wheel speed computation
- Publishes `TwistStamped` with proper `base_footprint` frame id
- Integrates with `ros2_control` `JointGroupVelocityController`
- Gazebo simulation via `ros_gz_sim` and `ros_gz_bridge`
<br />

## How It Works

### `arduino_joystick_controller` node
Reads raw analog X/Y values (0–1023) from Arduino over serial, normalizes to `[-1, 1]`, applies deadzone filtering, scales by configurable gain, and publishes as `geometry_msgs/TwistStamped` on `/bot_control/cmd_vel`.

| Parameter | Default | Description |
|---|---|---|
| `linear_gain` | 1.0 | Scales normalized joystick axis to linear velocity |
| `angular_gain` | 1.0 | Scales normalized joystick axis to angular velocity |

### `velocity_controller` node
Subscribes to `/bot_control/cmd_vel` and converts `(v, ω)` to individual left/right wheel speeds using differential drive inverse kinematics via Eigen.
Publishes to `/velocity_controller/commands` consumed by `ros2_control`.

| Parameter | Default | Description |
|---|---|---|
| `wheel_radius` | 0.033 | Wheel radius in metres |
| `wheel_separation` | 0.14 | Distance between wheels in metres |
<br />

## Project Structure

```
Control Layer (Core Logic)
├── bot_control/
│ ├── arduino_joystick_controller.cpp ← Serial → ROS2 (TwistStamped)
│ └── velocity_controller.cpp ← Kinematics → wheel velocity commands

System Integration
├── bot_bringup/
│ ├── controller.launch.py ← Launch control pipeline
│ ├── display.launch.py ← Gazebo + RViz2 setup
│ └── config/ ← Controllers + bridge + visualization

Robot Description
├── bot_description/
│ ├── urdf/ ← Robot model + ros2_control integration
| └── meshes/ ← STL files used only for reference
```
<br />

## Tech Stack

`ROS2` · `C++` · `Python` · `Arduino` · `Gazebo` ·

---

## Run

From the `ros2_ws` directory:

**Terminal 1 — Launch simulation:**
```bash
ros2 launch bot_bringup display.launch.py
```

Plug in the Arduino joystick over USB before launching Terminal 2.

**Terminal 2 — Launch controllers and joystick node:**
```bash
ros2 launch bot_bringup controller.launch.py
```

---

## System Architecture

<img src="/assets/rqt_graph.png" alt="RQT Graph">
*Node and topic connections*

<img src="/assets/tf2_tree.png" alt="TF2 Tree">
*Transform frame hierarchy*

<img src="/assets/gazebo_sim.png" alt="Gazebo Simulation">
*Differential drive robot in Gazebo*

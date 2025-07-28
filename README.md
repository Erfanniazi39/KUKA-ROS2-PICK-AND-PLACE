# KUKA ROS2 Pick and Place Project

This project implements a pick-and-place pipeline using the **KUKA KR70 R2100** industrial robot with a **Robotiq 2F-140** gripper in **ROS 2 Humble** and **MoveIt 2**, simulated in **Gazebo Fortress**.

## installation
```bash
#1.Clone the repo to your workspace
git clone https://github.com/Erfanniazi39/KUKA-ROS2-PICK-AND-PLACE.git
cd KUKA-ROS2-PICK-AND-PLACE

#2.Install Dependecies
Ensure you have all necessary dependencies installed:

```bash
sudo apt update
sudo apt install -y \
  ros-humble-moveit \
  ros-humble-ros-gz-sim \
  ros-humble-ros-gz-bridge



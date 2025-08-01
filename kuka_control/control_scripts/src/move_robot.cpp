/*
# ===================================== COPYRIGHT ===================================== #
#                                                                                       #
#  IFRA (Intelligent Flexible Robotics and Assembly) Group, CRANFIELD UNIVERSITY        #
#  Created on behalf of the IFRA Group at Cranfield University, United Kingdom          #
#  E-mail: IFRA@cranfield.ac.uk                                                         #
#                                                                                       #
#  Licensed under the Apache-2.0 License.                                               #
#  You may not use this file except in compliance with the License.                     #
#  You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0  #
#                                                                                       #
#  Unless required by applicable law or agreed to in writing, software distributed      #
#  under the License is distributed on an "as-is" basis, without warranties or          #
#  conditions of any kind, either express or implied. See the License for the specific  #
#  language governing permissions and limitations under the License.                    #
#                                                                                       #
#  IFRA Group - Cranfield University                                                    #
#  AUTHORS: Mikel Bueno Viso - Mikel.Bueno-Viso@cranfield.ac.uk                         #
#           Dr. Seemal Asif  - s.asif@cranfield.ac.uk                                   #
#           Prof. Phil Webb  - p.f.webb@cranfield.ac.uk                                 #
#                                                                                       #
#  Date: April, 2023.                                                                   #
#                                                                                       #
# ===================================== COPYRIGHT ===================================== #

# ======= CITE OUR WORK ======= #
# You can cite our work with the following statement:
# IFRA-Cranfield (2023) ROS 2 Sim-to-Real Robot Control. URL: https://github.com/IFRA-Cranfield/ros2_SimRealRobotControl.
*/

// Include standard libraries:
#include <string>
#include <vector>

// Include -> YAML file parser:
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <ament_index_cpp/get_package_share_directory.hpp>

// INCLUDE -> FUNCTIONS:
#include "control_scripts/move_joints.h"
#include "control_scripts/move_linear.h"
#include "control_scripts/move_xyzw.h"
#include "control_scripts/move_gripper.h"

// Include RCLCPP and RCLCPP_ACTION:
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

// Include MoveIt!2:
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

// Include the move ROS2 ACTION:
#include "control_actions/action/move.hpp"

// Include the ROS2 MSG messages:
// #include "control_actions/msg/joint.hpp"
#include "control_actions/msg/xyz.hpp"
#include "control_actions/msg/xyzypr.hpp"
#include "control_actions/msg/specs.hpp"

// Declaration of GLOBAL VARIABLES --> ROBOT / END-EFFECTOR / ENVIRONMENT PARAMETERS:
std::string param_ROB = "kuka_arm";
std::string param_EE = "robotiq_2f140";
std::string param_ENV = "gazebo";

// Declaration of GLOBAL VARIABLES --> MoveIt!2 Interface:
// moveit::planning_interface::MoveGroupInterface move_group_interface_ROB;
// moveit::planning_interface::MoveGroupInterface move_group_interface_EE;

moveit::planning_interface::MoveGroupInterface* move_group_interface_ROB = nullptr;
moveit::planning_interface::MoveGroupInterface* move_group_interface_EE = nullptr;


// Declaration of GLOBAL VARIABLES --> JointModelGroup:
const moveit::core::JointModelGroup* joint_model_group_ROB;
const moveit::core::JointModelGroup* joint_model_group_EE;

// Declaration of GLOBAL VARIABLE --> RES:
std::string RES = "none";

// Declaration of GLOBAL VARIABLES --> robotSPECS and eeSPECS:
control_actions::msg::Specs robotSPECS;
control_actions::msg::Specs eeSPECS;

// ======================================================================================================================== //
// ==================== PARAM: ROBOT + END-EFFECTOR ==================== //

class ros2_RobotParam : public rclcpp::Node
{
public:
    ros2_RobotParam() : Node("ros2_RobotParam") 
    {
        // this->declare_parameter("ROB_PARAM", "none");
        // param_ROB = this->get_parameter("ROB_PARAM").get_parameter_value().get<std::string>();
        // RCLCPP_INFO(this->get_logger(), "ROB_PARAM received -> %s", param_ROB.c_str());
    }
private:
};

class ros2_EEParam : public rclcpp::Node
{
public:
    ros2_EEParam() : Node("ros2_EEParam") 
    {
        // this->declare_parameter("EE_PARAM", "none");
        // param_EE = this->get_parameter("EE_PARAM").get_parameter_value().get<std::string>();
        // RCLCPP_INFO(this->get_logger(), "EE_PARAM received -> %s", param_EE.c_str());
    }
private:
};


// ======================================================================================================================== //
// ==================== FUNCTIONS ==================== //

// ===== PLAN ===== //
// ROBOT:
moveit::planning_interface::MoveGroupInterface::Plan plan_ROB() {
    
    moveit::planning_interface::MoveGroupInterface::Plan my_plan;
    bool success = (move_group_interface_ROB->plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);

    // Execute the plan
    if (success)
    {
        RES = "PLANNING: OK";
        return(my_plan);
    }
    else
    {
        RES = "PLANNING: ERROR";
        return(my_plan);
    }

};
// END-EFFECTOR:
moveit::planning_interface::MoveGroupInterface::Plan plan_EE() {
    
    moveit::planning_interface::MoveGroupInterface::Plan my_plan;
    bool success = (move_group_interface_EE->plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);

    // Execute the plan
    if (success)
    {
        RES = "PLANNING: OK";
        return(my_plan);
    }
    else
    {
        RES = "PLANNING: ERROR (EE)";
        return(my_plan);
    }
    
};


// ======================================================================================================================== //
// ==================== ACTION SERVER CLASS ==================== //

class ActionServer : public rclcpp::Node
{
public:
    using Move = control_actions::action::Move;
    using GoalHandle = rclcpp_action::ServerGoalHandle<Move>;

    explicit ActionServer(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
    : Node("MOVE_INTERFACE", options)
    {

        action_server_ = rclcpp_action::create_server<Move>(
            this,
            "/Move",
            std::bind(&ActionServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&ActionServer::handle_cancel, this, std::placeholders::_1),
            std::bind(&ActionServer::handle_accepted, this, std::placeholders::_1));

    }

private:
    rclcpp_action::Server<Move>::SharedPtr action_server_;
    
    // ACCEPT GOAL and NOTIFY which ACTION is going to be exectuted:
    rclcpp_action::GoalResponse handle_goal(
        [[maybe_unused]] const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const Move::Goal> goal)
    {
        std::string action = goal->action;

        if (action == "MoveJ") {
            auto MoveJGoal = goal->movej;
            RCLCPP_INFO(this->get_logger(), "Received a GOAL REQUEST: MoveJ Action -> (%.2f,%.2f,%.2f,%.2f,%.2f,%.2f)", MoveJGoal.joint1, MoveJGoal.joint2, MoveJGoal.joint3, MoveJGoal.joint4, MoveJGoal.joint5, MoveJGoal.joint6);
        }

        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE; 
    }

    // No idea about what this function does:
    void handle_accepted(const std::shared_ptr<GoalHandle> goal_handle)
    {
        // This needs to return quickly to avoid blocking the executor, so spin up a new thread:
        std::thread(
            [this, goal_handle]() {
                execute(goal_handle);
            }).detach();
        
    }

    // Function that cancels the goal request:
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandle> goal_handle)
    {
        RCLCPP_INFO(this->get_logger(), "Received a cancel request.");

        // We call the -> void moveit::planning_interface::MoveGroupInterface::stop(void) method,
        // which stops any trajectory execution, if one is active.
        if (param_ROB != "none"){
            move_group_interface_ROB->stop();
        }
        if (param_EE != "none" && param_ENV != "bringup"){
            move_group_interface_EE->stop();
        }

        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // MAIN LOOP OF THE ACTION SERVER -> EXECUTION:
    void execute(const std::shared_ptr<GoalHandle> goal_handle)
    {

        // Obtain ACTION type:
        const auto goal = goal_handle->get_goal();
        std::string action = goal->action;

        // DECLARE RESULT:
        auto result = std::make_shared<Move::Result>();

        // ===== ACTION EXECUTION ===== //
        moveit::planning_interface::MoveGroupInterface::Plan MyPlan;
        
        if (action == "MoveJ" && param_ROB != "none"){
            
            // 1. Define JP VECTOR:
            std::vector<double> JP;
            moveit::core::RobotStatePtr current_state = move_group_interface_ROB->getCurrentState(10);
            current_state->copyJointGroupPositions(joint_model_group_ROB, JP);
            
            // 2. CALL MoveJAction for CALCULATIONS:
            MoveJSTRUCT MoveJRES = MoveJAction(goal->movej, JP, robotSPECS);
            JP = MoveJRES.JP;
            move_group_interface_ROB->setJointValueTarget(JP);
            
            // 3. Assign SPEED and PLANNING METHOD (PTP, LIN, CIRC):
            move_group_interface_ROB->setMaxVelocityScalingFactor(goal->speed);
            move_group_interface_ROB->setPlannerId("PTP");

            // 4. PLAN:
            if (MoveJRES.RES == "LIMITS: OK"){
                MyPlan = plan_ROB();
            } else {
                RES = MoveJRES.RES;
            }

        } else if (action == "MoveL" && param_ROB != "none"){
            
            // 1. Define POSE VECTOR:
            auto POSE = move_group_interface_ROB->getCurrentPose();
            
            // 2. CALL MoveLAction for CALCULATIONS:
            auto TARGET_POSE = MoveLAction(goal->movel, POSE);
            move_group_interface_ROB->setPoseTarget(TARGET_POSE);
            
            // 3. Assign SPEED and PLANNING METHOD (PTP, LIN, CIRC):
            move_group_interface_ROB->setMaxVelocityScalingFactor(goal->speed);
            move_group_interface_ROB->setPlannerId("LIN");

            // 4. PLAN:
            MyPlan = plan_ROB();

        } else if (action == "MoveRP" && param_ROB != "none"){
            
            // 1. Define POSE VECTOR:
            auto POSE = move_group_interface_ROB->getCurrentPose();
            RCLCPP_INFO(this->get_logger(), "Current POSE before the new MoveXYZW was:");
            RCLCPP_INFO(this->get_logger(), "POSITION -> (x = %.2f, y = %.2f, z = %.2f)", POSE.pose.position.x, POSE.pose.position.y,POSE.pose.position.z);
            RCLCPP_INFO(this->get_logger(), "ORIENTATION (quaternion) -> (x = %.2f, y = %.2f, z = %.2f, w = %.2f)", POSE.pose.orientation.x, POSE.pose.orientation.y,POSE.pose.orientation.z,POSE.pose.orientation.w);

            
            // 2. CALL MoveRPAction for CALCULATIONS:
            auto TARGET_POSE = MoveRPAction(goal->moverp, POSE);
            RCLCPP_INFO(get_logger(), "Received a POSE GOAL request:");
            RCLCPP_INFO(this->get_logger(), "POSITION -> (x = %.2f, y = %.2f, z = %.2f)", TARGET_POSE.position.x, TARGET_POSE.position.y, TARGET_POSE.position.z);
            RCLCPP_INFO(this->get_logger(), "ORIENTATION (quaternion) -> (x = %.2f, y = %.2f, z = %.2f, w = %.2f)", TARGET_POSE.orientation.x, TARGET_POSE.orientation.y, TARGET_POSE.orientation.z, TARGET_POSE.orientation.w);
            move_group_interface_ROB->setPoseTarget(TARGET_POSE);
            
            // 3. Assign SPEED and PLANNING METHOD (PTP, LIN, CIRC):
            move_group_interface_ROB->setMaxVelocityScalingFactor(goal->speed);
            move_group_interface_ROB->setPlannerId("PTP");

            // 4. PLAN:
            MyPlan = plan_ROB();
        
        } else if (action == "MoveG" && param_EE != "none"){
            
            // 1. Define JP VECTOR:
            std::vector<double> JP;
            moveit::core::RobotStatePtr current_state = move_group_interface_EE->getCurrentState(10);
            current_state->copyJointGroupPositions(joint_model_group_EE, JP);

            // 2. CALL MoveGAction for CALCULATIONS:
            MoveGSTRUCT MoveGRES = MoveGAction(goal->moveg, JP, eeSPECS);
            JP = MoveGRES.JP;
            move_group_interface_EE->setJointValueTarget(JP);
            
            // 3. Assign SPEED and PLANNING METHOD (PTP, LIN, CIRC):
            move_group_interface_EE->setMaxVelocityScalingFactor(goal->speed);
            move_group_interface_EE->setPlannerId("geometric::RRTConnect");

            // 4. PLAN:
            if (MoveGRES.RES == "LIMITS: OK"){
                MyPlan = plan_EE();
            } else {
                RES = MoveGRES.RES;
            }
        
        }

        // EXECUTE:
        if (RES == "PLANNING: OK"){

            bool ExecSUCCESS = (move_group_interface_ROB->execute(MyPlan) == moveit::core::MoveItErrorCode::SUCCESS);

            if (goal_handle->is_canceling()) {
                RCLCPP_INFO(this->get_logger(), "Goal canceled.");
                result->result = action + ":CANCELED";
                goal_handle->canceled(result);
                return;
            } 
            
            if (ExecSUCCESS){
                RCLCPP_INFO(this->get_logger(), "%s - %s: Movement executed!", param_ROB.c_str(), action.c_str());
                result->result = action + ":SUCCESS";
                goal_handle->succeed(result);
            } else {
                RCLCPP_INFO(this->get_logger(), "%s - %s: Movement execution failed!", param_ROB.c_str(), action.c_str());
                result->result = action + ":FAILED. Reason -> Execution error.";
                goal_handle->succeed(result);
            }

        } else if (RES == "PLANNING: OK (EE)"){
            move_group_interface_EE->execute(MyPlan);

            if (goal_handle->is_canceling()) {
                RCLCPP_INFO(this->get_logger(), "Goal canceled.");
                result->result = action + ":CANCELED";
                goal_handle->canceled(result);
                return;
            } else {
                RCLCPP_INFO(this->get_logger(), "%s - %s: Movement executed!", param_EE.c_str(), action.c_str());
                result->result = action + ":SUCCESS";
                goal_handle->succeed(result);
            }
            
        } else if (RES == "PLANNING: ERROR"){
            RCLCPP_INFO(this->get_logger(), "%s - %s: Planning failed!", param_ROB.c_str(), action.c_str());
            result->result = action + ":FAILED. Reason -> Planning failed.";
            goal_handle->succeed(result);
        } else if (RES == "PLANNING: ERROR (EE)"){
            RCLCPP_INFO(this->get_logger(), "%s - %s: Planning failed!", param_EE.c_str(), action.c_str());
            result->result = action + ":FAILED. Reason -> Planning failed.";
            goal_handle->succeed(result);

        } else {

            RCLCPP_INFO(this->get_logger(), "ERROR: %s", RES.c_str());
            result->result = action + ":FAILED. Reason -> " + RES;
            goal_handle->succeed(result);

        };

        // RE-INITIALISE RES variable:
        RES = "none";

    }

};


// ==================== MAIN ==================== //

int main(int argc, char ** argv)
{
    // Initialise MAIN NODE:
    rclcpp::init(argc, argv);
    auto const logger = rclcpp::get_logger("MOVE_INTERFACE");

    // Obtain ROBOT + END-EFFECTOR + ENVIRONMENT parameters:
    auto node_PARAM_ROB = std::make_shared<ros2_RobotParam>();
    rclcpp::spin_some(node_PARAM_ROB);
    auto node_PARAM_EE = std::make_shared<ros2_EEParam>();
    rclcpp::spin_some(node_PARAM_EE);

    // DEFINE -> RobotSPECS + eeSPECS variables:
    // Robot SPECIFICATIONS:
    robotSPECS.robot_max = {180.0, 60.0, 165.0, 180.0, 125.0, 360.0};
    robotSPECS.robot_min = {-180.0, -174.0, -120.0, -180.0, -125.0, -360.0};

    // End-Effector SPECIFICATIONS:
    eeSPECS.ee_max = 0.70;
    eeSPECS.ee_min = 0.0;
    eeSPECS.ee_vector =  {1.0, -1.0, 1.0, 1.0, -1.0, 1.0};

    // Launch and spin (EXECUTOR) MoveIt!2 Interface node:
    auto name = "kuka_move";
    auto const node2 = std::make_shared<rclcpp::Node>(name, rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));
    rclcpp::executors::SingleThreadedExecutor executor; 
    executor.add_node(node2);
    std::thread([&executor]() { executor.spin(); }).detach();

    // CREATE -> MoveGroupInterface(s):
    using moveit::planning_interface::MoveGroupInterface;
    // 1. ROBOT:
    if (param_ROB != "none"){        
        //move_group_interface_ROB = MoveGroupInterface(node2, param_ROB);
        move_group_interface_ROB = new moveit::planning_interface::MoveGroupInterface(node2, param_ROB);
        move_group_interface_ROB->setPlanningPipelineId("pilz_industrial_motion_planner");
        move_group_interface_ROB->allowReplanning(true);
        move_group_interface_ROB->setPlanningTime(5.0);
        move_group_interface_ROB->allowReplanning(true);

        move_group_interface_ROB->setMaxVelocityScalingFactor(1.0);
        move_group_interface_ROB->setMaxAccelerationScalingFactor(1.0);

        joint_model_group_ROB = move_group_interface_ROB->getCurrentState()->getJointModelGroup(param_ROB);
        RCLCPP_INFO(logger, "MoveGroupInterface object created for ROBOT: %s", param_ROB.c_str());
    }
    // 2. END-EFFECTOR:
    if (param_EE != "none"){
        //move_group_interface_EE = MoveGroupInterface(node2, param_EE);
        move_group_interface_EE = new moveit::planning_interface::MoveGroupInterface(node2, param_EE);
        move_group_interface_EE->setPlanningPipelineId("ompl");
        move_group_interface_EE->allowReplanning(true);
        move_group_interface_EE->setPlanningTime(5.0);
        move_group_interface_EE->allowReplanning(true);

        move_group_interface_EE->setMaxVelocityScalingFactor(1.0);
        move_group_interface_EE->setMaxAccelerationScalingFactor(1.0);
        joint_model_group_EE = move_group_interface_EE->getCurrentState()->getJointModelGroup(param_EE);
        RCLCPP_INFO(logger, "MoveGroupInterface object created for END-EFFECTOR: %s", param_EE.c_str());
    }

    // CREATE -> PlanningSceneInterface:
    using moveit::planning_interface::PlanningSceneInterface;
    auto planning_scene_interface = PlanningSceneInterface();

    // Declare and spin ACTION SERVER:
    auto action_server = std::make_shared<ActionServer>();
    
    rclcpp::spin(action_server);
    delete move_group_interface_ROB;
    delete move_group_interface_EE;

    rclcpp::shutdown();
    return 0;
}

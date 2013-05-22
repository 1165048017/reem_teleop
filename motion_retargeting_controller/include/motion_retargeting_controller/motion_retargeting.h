/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef MOTION_RETARGETING_H_
#define MOTION_RETARGETING_H_

/*****************************************************************************
** Includes
*****************************************************************************/

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include <geometry_msgs/TransformStamped.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Empty.h>
#include <trajectory_msgs/JointTrajectoryPoint.h>

#include <motion_adaption/motion_adaption.h>
#include <tree_kinematics/tree_kinematics.h>
#include <tree_kinematics/GetTreePositionIK.h>

#include "motion_recorder.h"
#include "output_handler.h"

namespace motion_retargeting
{

class MotionRetargeting
{
public:
  /**
   * Initialies motion retargeting by configures motion adaption and tree kinematics
   */
  MotionRetargeting(const ros::NodeHandle& nh,
                      const motion_adaption::MotionAdaptionPtr motion_adaption,
                      const tree_kinematics::KinematicsParameters& kinematics_params,
                      const tree_kinematics::TreeKinematicsPtr tree_kinematics,
                      const MotionRecorderPtr motion_recorder,
                      const OutputHandlerPtr output_handler);
  /**
   * Throws out the trash
   */
  ~MotionRetargeting();
  /**
   * Does a one shot retargeting of the input task space motion to the output joint space motion
   * Adaption of the whole incoming motions needs to be completed before the IK can be calculated.
   * @param output_joint_states Contains a waypoint consisting of positions and velocities for each joint
   * @return true, if now error occurred
   *
   * TODO:
   *   * Current input (retrieveing tf transforms) is handled by motion adapation. Should we outsource that
   *     and make it the responsibility of the program using this class?
   *     * Pros: more flexible
   *     * Cons: need to checking of correct input and output each time -> takes time
   */
  bool retarget();

private:
  /**
   * ROS node handle
   */
  ros::NodeHandle nh_;
  /**
   * Motion adaption for adapting the input motion to the target system (e.g. robot)
   */
  motion_adaption::MotionAdaptionPtr motion_adaption_;
  /**
   * Adapted endpoint poses
   */
  std::vector<geometry_msgs::PoseStamped> adapted_entpt_poses_;
  /**
   * Tree IK for calculating the goal joint states
   */
  tree_kinematics::TreeKinematicsPtr tree_kinematics_;
  /**
   * Tree IK service request
   */
  tree_kinematics::GetTreePositionIK::Request tree_ik_request_;
  /**
   * Tree IK service response
   */
  tree_kinematics::GetTreePositionIK::Response tree_ik_response_;
  /**
   * Output handler for publishing the goal joint states
   */
  OutputHandlerPtr output_handler_;
  /**
   * Goal joint states sent to the output handler
   */
  sensor_msgs::JointState goal_joint_states_;
  /**
   * Subscriber for retrieving the current joint states, which are used as seed state for the IK calculations
   */
  ros::Subscriber joint_states_subscriber_;
  /**
   * Storage for last joint states
   */
  sensor_msgs::JointState joint_states_;
  /**
   * Flag for joint state initialisation. It is set to true, once first joint states have been received.
   */
  bool joint_states_initialised_;
  /**
   * Callback function for the joint states subscriber
   */
  void jointStatesCallback(const sensor_msgs::JointState::ConstPtr& msg);
  /**
   * Pointer to the motion recorder
   */
  MotionRecorderPtr motion_recorder_;
  /**
   * Subscriber for turning on and off the motion recorder
   */
  ros::Subscriber motion_recorder_subscriber_;
  /**
   * Callback function for the motion recorder subscriber
   */
  void motionRecorderCallback(const std_msgs::Empty::ConstPtr& msg);
  /**
   * Flag for indicating if motion recording is in progress
   */
  bool record_motion_;
  /**
   * Subscriber for turning on and off output processing
   */
  ros::Subscriber output_control_subscriber_;
  /**
   * Callback function for turning on/off processing the output of motion retargeting
   */
  void outputControlCallback(const std_msgs::Empty::ConstPtr& msg);
  /**
   * Flag for indicating if motion recording is in progress
   */
  bool process_output_;
};

typedef boost::shared_ptr<MotionRetargeting> MotionRetargetingPtr;

} // namespace motion_retargeting

#endif /* MOTION_RETARGETING_H_ */

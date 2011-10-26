
#ifndef COLLVOID_LOCAL_PLANNER_H_
#define COLLVOID_LOCAL_PLANNER_H_


#include <tf/transform_listener.h>
#include <nav_core/base_local_planner.h>
#include <angles/angles.h>
#include <nav_msgs/Odometry.h>
#include <ros/ros.h>
#include <boost/unordered_map.hpp>

#include "collvoid_local_planner/ROSAgent.h"
#include "collvoid_local_planner/pose_twist_aggregator.h"


namespace collvoid_local_planner {

  enum State
  {
    INIT,
    RUNNING,
    STOPPED,
    AT_GOAL // implies stopped
  };


  class CollvoidLocalPlanner: public nav_core::BaseLocalPlanner {
  public:
    CollvoidLocalPlanner();
    CollvoidLocalPlanner(std::string name, tf::TransformListener* tf, costmap_2d::Costmap2DROS* costmap_ros);
    ~CollvoidLocalPlanner();
    void initialize(std::string name, tf::TransformListener* tf, costmap_2d::Costmap2DROS* costmap_ros);
    bool isGoalReached();
    bool setPlan(const std::vector<geometry_msgs::PoseStamped>& global_plan);
    void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);
    bool computeVelocityCommands(geometry_msgs::Twist& cmd_vel);

  private:
    bool rotateToGoal(const tf::Stamped<tf::Pose>& global_pose, const tf::Stamped<tf::Pose>& robot_vel, double goal_th, geometry_msgs::Twist& cmd_vel);
    bool stopWithAccLimits(const tf::Stamped<tf::Pose>& global_pose, const tf::Stamped<tf::Pose>& robot_vel, geometry_msgs::Twist& cmd_vel);

    double calculateMaxTrackSpeedAngle(double T, double theta, double error);
    double calcVstarError(double T,double theta, double error);
    double calcVstar(double vh, double theta);
    double vMaxAng();
    double beta(double T, double theta);
    double gamma(double T, double theta, double error);

    void addAllNeighbors();
    void updateROSAgentWithMsg(ROSAgent* agent, collvoid_msgs::PoseTwistWithCovariance* msg);

    double sign(double x){
      return x < 0.0 ? -1.0 : 1.0;
    }


    //Datatypes:
    costmap_2d::Costmap2DROS* costmap_ros_; 
    tf::TransformListener* tf_; 
    
    PoseTwistAggregator* pt_agg_;

    bool initialized_;

    double sim_period_;
    double max_vel_x_, min_vel_x_;
    double max_vel_y_, min_vel_y_;
    double max_vel_th_, min_vel_th_;
    double acc_lim_x_, acc_lim_y_, acc_lim_theta_;
    double xy_goal_tolerance_, yaw_goal_tolerance_;
    double rot_stopped_velocity_, trans_stopped_velocity_;
    double inscribed_radius_, circumscribed_radius_, inflation_radius_; 

    double wheel_base_;

    bool latch_xy_goal_tolerance_, xy_tolerance_latch_, rotating_to_goal_;
    
    int current_waypoint_;
    //params ORCA
    bool use_ground_truth_, scale_radius_;
    double  neighbor_dist_, time_horizon_,time_horizon_obst_;
    int max_neighbors_;
    bool holo_robot_;
    double INIT_GUESS_NOISE_STD_,THRESHOLD_LAST_SEEN_;
    int MAX_INITIAL_GUESS_;
    int nr_initial_guess_;
    State state_;
    ROSAgent* me_;
    //boost::unordered_map<std::string,ROSAgent> neighbors_;
   

    std::string global_frame_; ///< @brief The frame in which the controller will run
    std::string robot_base_frame_; ///< @brief Used as the base frame id of the robot
    std::vector<geometry_msgs::PoseStamped> global_plan_, transformed_plan_;
  

  };//end class

  RVO::Vector2 rotateVectorByAngle(double x, double y, double ang);


}; //end namespace

#endif //end guard catch
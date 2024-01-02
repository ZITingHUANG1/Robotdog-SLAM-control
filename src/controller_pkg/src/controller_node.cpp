#include "controller_node.h"

class controllerNode{
  ros::NodeHandle nh;
  tf::StampedTransform transform;
  ros::Subscriber desired_state, current_state,current_goal;
  ros::Publisher commands;
  ros::Timer timer;

  // Controller internals (you will have to set them below)
  // Current state
  Eigen::Vector3d x;     // current position of the UAV's c.o.m. in the world frame
  Eigen::Vector3d v;     // current velocity of the UAV's c.o.m. in the world frame
  Eigen::Matrix3d R;     // current orientation of the UAV

  // Desired state
  // Eigen::Vector3d xd;    // desired position of the UAV's c.o.m. in the world frame
  // Eigen::Vector3d vd;    // desired velocity of the UAV's c.o.m. in the world frame
  // Eigen::Vector3d ad;    // desired acceleration of the UAV's c.o.m. in the world frame
  double yawd;           // desired yaw angle
  double pitchd;

  int gait;
  double xd;
  double yd;
  double yaw;
  double hz;             // frequency of the main control loop
  double x_current;       
  double y_current;
  double z_current;
  double x_goal;
  double y_goal;
  double start;
  double now;
  double timeperiod;
  double roll,pitch;
  double yaw_thr = 0.2;
  bool flag = true;
  bool parkour = false;


public:
  controllerNode():hz(100.0){
      desired_state = nh.subscribe("move_base/TrajectoryPlannerROS/global_plan", 2, &controllerNode::onDesiredState, this);
      current_state = nh.subscribe("current_state_est", 1, &controllerNode::onCurrentState, this);
      current_goal = nh.subscribe("current_goal", 10, &controllerNode::goal_controller, this);
      commands = nh.advertise<mav_msgs::Actuators>("commands", 1);
      timer = nh.createTimer(ros::Rate(hz), &controllerNode::controlLoop, this);
  }
  void onDesiredState(const nav_msgs::Path& des_state){
      // state.poses[5].pose.position.y,des_state.poses[5].pose.position.z;
      xd = des_state.poses[5].pose.position.x;
      yd = des_state.poses[5].pose.position.y;
      tf::Quaternion q;
      yawd = tf::getYaw(q);
      if (pitch < -0.18 || pitch >0.18){
        yawd = 0;
      }
  }

  void onCurrentState(const nav_msgs::Odometry& cur_state){
    x_current = cur_state.pose.pose.position.x;
    y_current = cur_state.pose.pose.position.y;
    z_current = cur_state.pose.pose.position.z;

    x << cur_state.pose.pose.position.x,cur_state.pose.pose.position.y,cur_state.pose.pose.position.z;
    tf::Quaternion quat;
    tf::quaternionMsgToTF(cur_state.pose.pose.orientation,quat);
    tf::Matrix3x3(quat).getRPY(roll,pitch,yaw);
  }

  void goal_controller(const geometry_msgs::PoseStamped& goal){
    // x_goal << goal.pose.position.x, goal.pose.position.y, goal.pose.position.z;
    x_goal = goal.pose.position.x;
    y_goal = goal.pose.position.y;
  }



  void controlLoop(const ros::TimerEvent& t){
    ros::Rate rate(1);
    double ex,ey,eyaw;

    //position error
    ex = xd - x_current;
    ey = yd - y_current;

    //yaw angle error
    double angle_rad =atan2(ey,ex);
    eyaw = M_PI/2 - angle_rad + yaw;
    // eyaw = 0.05;  //for test

    //PRINT ALL INFO
    // current state
    std::cout<<"height: "<<z_current<<std::endl;
    std::cout<<"pitch: "<< pitch <<std::endl;

    std::cout<<"current x: "<<x_current<<std::endl;
    std::cout<<"current y: "<<y_current<<std::endl;
    std::cout<<"current yaw: "<<yaw<<std::endl;
    // desired state
    std::cout<<"desired x: "<<xd<<std::endl;
    std::cout<<"desired y: "<<yd<<std::endl;
    std::cout<<"desired yaw: "<<angle_rad<<std::endl;
    // error
    std::cout<<"current error of x: "<<ex<<std::endl;
    std::cout<<"current error of y: "<<ey<<std::endl;
    std::cout<<"error of the yaw: "<<eyaw<<std::endl;
    std::cout<<"---------------------------"<<std::endl;

    double goal_error = (x_current-x_goal)*(x_current-x_goal)+(y_current-x_goal)*(y_current-x_goal);
    mav_msgs::Actuators msg;

    //check if the robots reaches parkour terrain
    if (x_current < 6 && x_current > 4.8 && y_current > 1.40 && y_current < 1.55 && flag == true){
      flag = false;
      parkour = true;

      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 90; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
      std::cout<<"aim!aim!aim!aim!aim!aim!aim!aim!aim!"<<std::endl;
      commands.publish(msg);
      ros::Duration(2).sleep();
      
    // climb the stairs (front legs)
      std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<std::endl;
      for(int i=0;i<5;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 0;
        msg.angular_velocities[2] = 3; // Amplitude
        msg.angular_velocities[3] = 25; 
        msg.angular_velocities[4] = 12; 
        commands.publish(msg);
        std::cout<<"climbing!climbing!climbing!climbing!climbing!"<<std::endl;
        ros::Duration(3.5).sleep();
      }

      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 90; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 7; // Frequency of legs
      commands.publish(msg);
      ros::Duration(3).sleep();

    // climb the stairs (back legs)
      for (int i = 0;i<2;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 0;
        msg.angular_velocities[2] = 3; // Amplitude
        msg.angular_velocities[3] = 38; 
        msg.angular_velocities[4] = 10; 
        commands.publish(msg);
        std::cout<<"period 2period 2period 2period 2period 2period 2"<<std::endl;
        ros::Duration(2).sleep();      
        }

    // ready to go downstairs
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 90; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      std::cout<<"goooooooooooooooooooooooooooo"<<std::endl;
      commands.publish(msg);
      ros::Duration(3).sleep();
      
    // Go ramp
      for(int i=0;i<7;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 0;
        msg.angular_velocities[2] = 3; // Amplitude
        msg.angular_velocities[3] = 25; 
        msg.angular_velocities[4] = 10; 
        commands.publish(msg);
        std::cout<<"ramp!ramp!ramp!ramp!ramp!ramp!ramp!"<<std::endl;
        ros::Duration(3.5).sleep();}
      for(int i=0;i<5;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 90;
        msg.angular_velocities[2] = 0; // Amplitude
        msg.angular_velocities[3] = 4; 
        msg.angular_velocities[4] = 6; 
        commands.publish(msg);
        std::cout<<"downdowndowndowndowndowndowndown"<<std::endl;
        ros::Duration(1).sleep();
        }

    // to second stair
      for(int i=0;i<12;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 0;
        msg.angular_velocities[2] = 3; // Amplitude
        msg.angular_velocities[3] = 25; 
        msg.angular_velocities[4] = 10; 
        commands.publish(msg);
        std::cout<<"current y: "<<y_current<<std::endl;
        std::cout<<"ramp!ramp!ramp!ramp!ramp!ramp!ramp!"<<std::endl;
        ros::Duration(3.5).sleep();}

      for (int i = 0;i<2;i++){
        msg.angular_velocities[0] = 10; 
        msg.angular_velocities[1] = 10;
        msg.angular_velocities[2] = -4; // Amplitude
        msg.angular_velocities[3] = 40; 
        msg.angular_velocities[4] = 10; 
        commands.publish(msg);
        std::cout<<"period 2period 2period 2period 2period 2period 2"<<std::endl;
        ros::Duration(2).sleep();      
      }
    }

    // second stair
    else if (!flag && y_current < 6.8 && parkour){
      for (int i = 0;i<3;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 0;
        msg.angular_velocities[2] = 4; // Amplitude
        msg.angular_velocities[3] = 33; 
        msg.angular_velocities[4] = 7; 
        commands.publish(msg);
        std::cout<<"current y: "<<y_current<<std::endl;
        std::cout<<"period 2period 2period 2period 2period 2period 2"<<std::endl;
        ros::Duration(1).sleep();      
        }

      for(int i=0;i<3;i++){
        msg.angular_velocities[0] = 0; 
        msg.angular_velocities[1] = 90;
        msg.angular_velocities[2] = 0; // Amplitude
        msg.angular_velocities[3] = 0; 
        msg.angular_velocities[4] = 8; 
        commands.publish(msg);
        std::cout<<"current y: "<<y_current<<std::endl;
        std::cout<<"walk!walk!walk!walk!walk!walk!walk!walk!walk!walk!"<<std::endl;
        ros::Duration(2.5).sleep();
        }
      parkour = false;
      }

    // Check the direction - Go Straight
    if (eyaw > -yaw_thr && eyaw < yaw_thr && !parkour){
      gait = 1;
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 90; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      std::cout<<"gait1: straight!straight!straight!straight!straight!"<<std::endl;

      commands.publish(msg);
      ros::Duration(5).sleep();
    }

    // Yaw error less than treshould - Turn Left
    else if(eyaw < -yaw_thr && eyaw > -350*(2*M_PI)/360 && !parkour){
      gait = 2;
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = -45; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
      std::cout<<"gait2: left!left!left!left!left!left!left!left!"<<std::endl;
    }

    // Turn Around
    else if(eyaw < -350*(2*M_PI)/360 && !parkour ){
      gait = 6;
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = -45; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
      std::cout<<"gait6: turn around!turn around!turn around!turn around!turn around!"<<std::endl;
      ros::Duration(10).sleep();
    }

    // yaw error greater than treshould - Turn Right
    else if(eyaw > yaw_thr && eyaw < 350*(2*M_PI)/360 && !parkour){
      gait = 3;
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 45; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
      std::cout<<"gait3: right!right!right!right!right!right!right!"<<std::endl;
    }

    //turn around
    else if(eyaw > 350*(2*M_PI)/360 && !parkour){
      gait = 6;
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 45; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
      std::cout<<"gait6: turn around!turn around!turn around!turn around!turn around!"<<std::endl;
      ros::Duration(10).sleep();
    } 

    //tbd
    else{
      msg.angular_velocities.resize(5);
      msg.angular_velocities[0] = 0; // Phase between front and back legs (in degree)
      msg.angular_velocities[1] = 90; // Phase between front left + back right legs and front right and left back legs
      msg.angular_velocities[2] = 0; // Amplitude change of all legs
      msg.angular_velocities[3] = 0; // Amplitude change of back legs (added to angular_velocities[2])
      msg.angular_velocities[4] = 8; // Frequency of legs
      commands.publish(msg);
    }
  }
};

int main(int argc, char** argv){
  ros::init(argc, argv, "controller_node");
  ROS_INFO_NAMED("controller", "Controller started!");
  controllerNode n;
  ros::spin();
}
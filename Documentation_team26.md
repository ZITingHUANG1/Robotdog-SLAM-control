# Introduction to ROS - Group 26

> ** The Documentation must include the following elements**:
>  2) which requirements we couldn't fulfuill and why (explain the problems)
>  3) who of the team members wworked on which components
>  4) Ros graph, including who worked on which part
>  5) figures, and plots presenting our results
>  6) a bibliography
 
# 1. Group Members
 
|     Name       | Matriculation Nr. |   LRZ   |
|----------------|-------------------|---------|
| Garam Lee      | 03749414          | ge23gax |
| Ye Jin         | 03776973          | ge92dut |
| Ziting Huang   | 03754936          | ge95qus |
| Chen Yao       | 03739235          | ge96ved |
| Chenyuan Chen  | 03765920          | ge89mos |


# 2. Overview

## 2.1 ROS Package and Nodes

|      Package      |          Nodes                |         Functionality                                                      |
|-------------------|-------------------------------|----------------------------------------------------------------------------|
| depth_image_proc  | /nodelet_manager, /nodelet1   | convert depth image into point cloud                                       |
| octomap           | /octomap_server               | build an occupancy map based on the point cloud                            |
| move_base         | /move_base                    | navigate the robot from its current position to the goal position          |
| controller_pkg    | /controller_node              | control the robodog to reach the goal position by following the path plan  |


## 2.2 ROS Graph
It is our rqt Graph to display the communication graph between ROS nodes.

![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/rqt.PNG)

## 2.3 Performance

In the beginning, each member worked on the specific parts as shown in the table below. 

** Member in charge** 
|  Relevant node in ROS graph  |        Member in change            |
|------------------------------|------------------------------------|
| /nodelet_manager, /nodelet1  | Garam Lee, Ziting Huang            |
| /octomap_server              | Garam Lee, Ziting Huang, Chen Yao  |
| /move_base                   | Garam Lee, Ziting Huang, Chen Yao  |
| /controller_node             | Ye Jin, Chen Yao, Chenyuan Chen    |

During the implementation of the controller, parts of paramter values were adjusted by the members who worked on /controller_node. 
Finally, we modified the codes together to solve problems and to improve the performance of the robodog.

 
# 3. Process Flow

## 3.1 Perception Pipeline and Map Generation

Name of the launch file: 3D_OctoMap.launch 
Packages used: depth_image_proc, octomap

By running the launch file, the two nodes will be run: /nodelet, /octomap_server

### 3.1.1 nodelet_manager

With Nodelet, multiple nodes can be combined and executed within the same process. These merged nodes communicate through direct memory sharing within the process, eliminating the inter-process communication overhead. 

Here, by using Nodelet, point cloud data processing becomes more efficient within the system.

This node 
- subscribes to the topics **/realsense/depth/Camera_info** and **/realsense/depth/image** for depth image.
- publishes XYZ point cloud through the topic **/point_cloud**.

### 3.1.2 octomap_server

It is to build a 3D-occupancy map based on point cloud.

This node 
- subscribes to the topic **/point_cloud** for point cloud
- publishes the two topics **/projected_map** and **/octomap_binary**. 
  - The topic **/projected_map** is a 2D occupancy map from the 3D map
  - The topic **/octomap_binary** encodes free and occupied spaces.
  Both topics can be used to visualize the map and occupancy map in RVIZ, respectively.
  
By adjusting the value of
- "sensor_model/max_range" as 2.0, unnecessary points of point cloud are shown in the map.
- "pointcloud_min_z" as 0.10, obstacles can be well detected while the floor to the goal is percived as a free space.
- "filter ground", the ground will not be generated into point cloud, it can potentially aviod to regard ramp and stairs as obstacles.


## 3.2 Path Planning

Name of the launch file: Navigation.launch
Packages used: move_based

By running the launch file, a node will be run: move_base

There are four yaml files to be generated before running the node: base_local_planner_params.yaml, costmap_common_params.yaml,graph_planner_params.yaml, global_costmap_params.yaml and local_costmap_params.yaml. Configuration options are specified for the robot to reach the goal here.

### 3.2.1 base_local_Planner_params.yaml
The base_local_Planner_params.yaml file is used to configure parameters for the base local planner module in a robot's navigation stack. The base local planner is responsible for computing and executing trajectories to move the robot safely and efficiently within its local environment. This YAML file allows to adjust various settings to fine-tune the planner's behavior according to the robot's characteristics and the specific navigation requirements.

### 3.2.2 costmap_common_params.yaml
The costmap_params.yaml file contains configuration parameters for setting up the costmap, which is an essential component of the robot's navigation system. The costmap represents the occupancy status of the robot's environment, allowing it to plan safe and collision-free paths. This document explains each parameter and its significance, such as: 

To set the size of the robot and the obstacles, when the robot cannot across the narrow passage, decrease 'inflation_radius'. 
'allow_unknown' specifies whether or not to allow the planner to create path that traverse unknown space.

### 3.2.3 graph_planner_params.yaml
The GraphPlanner is a component used for path planning and navigation in a robot's navigation stack. This configuration file provides the necessary parameters to adjust the behavior of the GraphPlanner module according to the robot's specific requirements. Below are the parameters and their descriptions.

### 3.2.4 global_costmap_params.yaml
This global_costmap_params.yaml file is to specify the frequencies for updating and publishing the global costmap, and the global map size. These parameters are related to your pc computation power.

- **track_unknown_space** set to true to not allow the robot to traverse unkonwn space.
- 'resolution' This value should correspond to the resolution of the map, the same applies to **local_costmap_params.yaml**

### 3.2.5 local_costmap_params.yaml
Similar to the parameters in *global_costmap_params.yaml*, but for the local map, usually smaller than global map.

### 3.2.6 navfn_global_planner_params.yaml
This navfn_global_planner_params.yaml file is a global path planner used in a robot's navigation stack. It uses the Dijkstra's algorithm to find an optimal path from the robot's current position to the desired goal position. This configuration file provides the necessary parameters to adjust the behavior of the NavfnROS module according to the robot's specific requirements. Below are the parameters and their descriptions.

- **visualize_potential**: A boolean value that determines whether to visualize the potential grid. When set to true, the potential grid will be published and can be visualized for debugging and visualization purposes.

- **allow_unknown**: A boolean value that determines whether to allow unknown space in the global path planning. When set to true, the planner will consider unknown areas in the map during path planning.

- **planner_window_x**: Represents the X size of the potential grid around the robot. Setting this value to 0 means that the entire costmap will be used for global path planning.

- **planner_window_y**: Represents the Y size of the potential grid around the robot. Setting this value to 0 means that the entire costmap will be used for global path planning.

- **default_tolerance**: Specifies the default tolerance for the goal reaching or waypoint completion. It determines how close the planner should get to the desired goal or waypoints before considering them as successfully reached.

### 3.2.7 dwa_local_planner_params.yaml(not used)
This dwa_local_planner_params.yaml file is a component used for dynamic window approach (DWA) based trajectory planning and navigation in a robot's navigation stack. This configuration file provides the necessary parameters to adjust the behavior of the DWAPlannerROS module according to the robot's specific requirements. Main changes are the robot configuration parameters and trajectory scoring parameters.

NOTE: if you are using a layered costmap_2d costmap with a voxel or obstacle layer, you must also set the track_unknown_space param for that layer to be true, or it will convert all of your unknown space to free space (which navfn will then happily go right through).


## 3.3 Control and State Machine

The Control of the quadruped is done by the node ***control_node.cpp***. And the state machine means the transition system between different gaits, the details are following.

### 3.3.1 Control
control_node subscribes to topics **/current_state_est** and **/move_base/TrajectoryPlannerROS/global_plan**, and publishes **/commands**.

Before reaching the area front of the stairs, After the 2D-Goal being published in rviz by left-clicking on the map. ***Move_base*** generates continuously the path and the intermediate points, the control_node subscribes the current pose and the goal pose. The yaw angle diffenrence is the only control rule in our work. In ***control_node.cpp***, the vector in direction to the goal position in world coordinates is calculated, by calculating the error of the yaw angle of the robot and the vector, the error of the yaw angle is known.

- To avoid the jittering of the robot, when the robot starts to go straight, we force the robot to stay in this gait longer;

### 3.3.2 State Machine
Since the error of the yaw angle is known, when it exceeds the threshould we defined in advance, the robot changes its gait from *go straight* to *turn left* ,*turn right*, *turn around* or *traverse terrain*. 

In each gait, the phase difference between four legs is set with different parameters. And the parameters are determined by trial-and-error way. Hence, the robot's movement may not be most perfect.

# 4. Results

## 4.1 Octomap

1. At the beginning:

   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/ditu_in%20the%20beginning.PNG)

2. At second stair:

   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp8.PNG)

## 4.2 Path planning
Because of unknown part in the map, we need to click the 2D Nav Goal several times.
1. First:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp1.PNG)
2. Second:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp2.PNG)
3. Third: 


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp3.PNG)
4. Fourth: 


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp4.PNG)
5. Fifth: 


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp5.PNG)
6. Sixth: 


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp6.PNG)
7. Seventh:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp7.PNG)
8. Eighth:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/pathp8.PNG)

## 4.3 Movement
1. Movement through the corridor:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state5.PNG).

2. Turning movement to right and to left:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state6.PNG).

3. Ascending one-step stairs with the front feet leading and followed by the rear feet：


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state1.PNG).


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state2.PNG).

4. Descending one-step stairs with the front feet leading and followed by the rear feet：


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state3.PNG).

5. Walking through a slope:


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/state4.PNG).

6. Ascending two-step stairs with the front feet leading and followed by the rear feet：


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/obs3.1.jpeg).

7. Descending two-step stairs with the front feet leading and followed by the rear feet：


   ![](https://gitlab.lrz.de/00000000014B2E2F/team26_intro2ros_2023/-/raw/main/results/obs3.2.jpeg).


## 5. Problem   

1. Finding an optimal value for **inflation_radius** is challenging. In narrow passages, the robot's large size and a large inflation_radius cause it to consider the passage impassable. 

2. When using DWA as the local planner, it cannot find a path to reach the 2D goal. It is unclear whether the problem lies with the DWA planner itself or if there are errors in **base_local_planner.yaml** parameters.

3. Finding a perfect solution for the gait during ascending and descending stairs is indeed challenging. This is because the height and slope of the stairs can lead to gait instability or difficulties in balancing for the robot.

4. When descending stairs, the camera angle facing the ground is too large, causing the ground to be perceived as an obstacle. As a result, after descending the stairs, the robot cannot find a suitable path to reach the goal.

## 6. Future

1. AI can be used to recognize steps and trigger different gaits.
2. More optimized localization methods like Kalman filter and particle filter can be employed.
3. Other mapping methods can be used, such as ORB-SLAM's or other Visual Simultaneous Localization and Mapping (VSLAM) algorithms.

## 7. Bibliograph

Point cloud
   <http://wiki.ros.org/nodelet/Tutorials/Running%20a%20nodelet>
   <http://wiki.ros.org/depth_image_proc>
  
Octomap 
   <https://github.com/OctoMap/octomap_mapping/blob/kinetic-devel/octomap_server/launch/octomap_mapping.launch>
   <http://wiki.ros.org/octomap_server>

Path planning
   <http://wiki.ros.org/move_base>
   <http://wiki.ros.org/navigation/Tutorials/RobotSetup>
   <http://wiki.ros.org/base_local_planner>
   <http://wiki.ros.org/costmap_2d>
  

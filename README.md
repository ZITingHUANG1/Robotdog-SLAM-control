# Steps to run the Project

## 1.Download the project

1. Git clone the code from our [repository](git@gitlab.lrz.de:00000000014B2E2F/team26_intro2ros_2023.git).
2. run `cd team26_intro2ros_2023`
3. run `catkin build` or `catkin build -DPYTHON_EXECUTABLE=/usr/bin/python3`
4. Download Quadruped.zip from this [link](https://syncandshare.lrz.de/getlink/fiEg9ocZ6Pc5iuEa4QqN1b/)
5. Unzip the Unity file and copy the files to team26_intro2ros_2023/devel/lib/simulation/

## 2.Download the external ROS package

Because we don't need change the code of these Packags,so we can directly download their binary files.

1. pkg:`depth_image_proc`

   ```
   sudo apt-get install ros-noetic-image-pipeline 
   ```

2. pkg:`Octomap`

   ```
   sudo apt-get install ros-noetic-octomap
   sudo apt-get install ros-noetic-octomap-server
   sudo apt-get install ros-noetic-octomap-rviz-plugins
   ```

3. pkg:`move_base`

   ```
   sudo apt-get install ros-noetic-move-base
   ```

4. pkg:`map_server`

   ```
   sudo apt-get install ros-noetic-map-server
   ```

## 3.Run the project

Please run the following commands in the shell:

1. Add environment: `vim ~/.bashrc`

   Then add `source /home/$(your pc name)/team26_intro2ros_2023/devel/setup.bash`to the file bottom 

2. Run the Project: `roslaunch simulation final_robodog.launch`

3. Use the button `2D Nav Goal` in the Rviz to choose an location, where we want dog go.

docker run \
  --rm \
  --user 1000:1000 \
  -e ROS_DOMAIN_ID=0 \
  -e RMW_IMPLEMENTATION=rmw_fastrtps_cpp \
  -e DISPLAY=$DISPLAY  \
  -e QT_X11_NO_MITSHM=1 \
  -e TURTLEBOT3_MODEL=waffle_pi \
  -e GAZEBO_MODEL_PATH=/usr/share/gazebo-11/models:/home/felaze/ros2_ws/world/ \
  --device /dev/dri/card1:/dev/dri/card1 \
  --device /dev/dri/renderD128:/dev/dri/renderD128 \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  -v $HOME/ros2_ws:/home/felaze/ros2_ws:rw \
  -v `pwd`/generated/PickAndPlace/ros:/home/felaze/ros2_ws/src/koda_ros:rw \
  -w /home/felaze/ros2_ws \
  ros2:v1.0.0 \
  bash \
  -ic \
  "source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch koda_ros koda-extra.launch.py"
docker run -it \
  --name ubuntu22 \
  --user 1000:1000 \
  --net=host \
  -e DISPLAY=$DISPLAY \
  -e QT_X11_NO_MITSHM=1 \
  --device /dev/dri/card1:/dev/dri/card1 \
  --device /dev/dri/renderD128:/dev/dri/renderD128 \
  --group-add 44 \
  --group-add 992 \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  -v "$PWD":/home/ubuntu/maki:rw \
  -v "$HOME/ros2_ws":/home/ubuntu/ros2_ws:rw \
  maki-jazzy:v1.0.0
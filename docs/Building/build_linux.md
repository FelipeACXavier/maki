@page building_linux Building Linux

The linux setup has been tests on Ubuntu 24.04, that is also what we use in the docker. When I have time, I will try to make releases for other distros.

## Building the application

A Dockerfile is provided to ensure everyone has the same build and run environment. Note that there is no specific run image though. To build the application, follow the instructions below:

<ol start="1"> <li>Build the docker container, this shouldn't take long (30 minutes or so), since we use precompiled QT libraries. In any case, thi only needs to be done once.</li> </ol>

```bash
docker build . \
  --build-arg USERNAME=$(id -un) \
  -f docker/maki \
  -t maki:v1.0.0
```

<ol start="2"> <li>Run the docker image.</li></ol>

```bash
docker run -it \
  --name maki \
  --net=host \
  -e DISPLAY=:0 \
  -e QT_X11_NO_MITSHM=1 \
  --device /dev/dri \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  -v .:/home/ubuntu/maki:rw \
  -v ~/ros2_ws:/home/ubuntu/ros2_ws:rw \
  maki:v1.0.0
```

<ol start="3"> <li>Inside the docker, we can now build Maki. There are two options available</li></ol>

```bash
./scripts/linux/build.sh
```

  - If you need support for clangd and `compile_commands.json`, you can pass the options `--local-qt` and `--local-project`, which will make sure the generated `compile_commands.json` points to your local QT installation and project folder. For example:
```bash
./scripts/linux/build.sh --local-qt /opt/qt/6.8.3/gcc_64 --local-project /home/foo/maki
```


<ol start="4"> <li>After building, it is possible to run the application with:</li></ol>

```bash
./build/linux/debug/app/maki
```

## Release version

<ol start="1"> <li>To create a release version, run the command</li></ol>

```bash
./scripts/linux/build.sh --release
```

<ol start="2"> <li>After building, it is possible to run the application with:</li></ol>

```bash
./release/linux/bin/maki
```

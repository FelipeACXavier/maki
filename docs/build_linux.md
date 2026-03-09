# Linux setup

The linux setup has been tests on Ubuntu 24.04, that is also what we use in the docker. When I have time, I will try to make releases for other distros.

## Building the application

A Dockerfile is provided to ensure everyone has the same build and run environment. Note that there is no specific run image though. To build the application, follow the instructions below:

1. Build the docker container, this shouldn't take long (30 minutes or so), since we use precompiled QT libraries. In any case, thi only needs to be done once.

```bash
docker build . \
  --build-arg USERNAME=$(id -un) \
  -f docker/maki \
  -t maki:v1.0.0
```

2. Run the docker image

```bash
docker run -it \
  --name maki \
  --user 1000:1000 \
  --net=host \
  -e DISPLAY=:0 \
  -e QT_X11_NO_MITSHM=1 \
  --device /dev/dri \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  -v .:/home/$(id -un)/maki:rw \
  -v ~/ros2_ws:/home/$(id -un)/ros2_ws:rw \
  maki:v1.0.0
```

3. Inside the docker, we can now build Maki. There are two options available

```bash
./scripts/linux/build.sh 
```

  - If you need support for clangd and `compile_commands.json`, you can pass the options `--local-qt` and `--local-project`, which will make sure the generated `compile_commands.json` points to your local QT installation and project folder. For example:
```bash
./scripts/linux/build.sh --local-qt /opt/qt/6.8.3/gcc_64 --local-project /home/foo/maki
```

4. Finally, to install the tool

```bash
./scripts/linux/release.sh
```

## Running

After the installation described above, it is possible to run the application with:

```bash
./release/linux/bin/maki
```
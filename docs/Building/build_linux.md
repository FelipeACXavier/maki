@page building_linux Building Linux

The linux setup has been tests on Ubuntu 24.04, that is also what we use in the docker. When I have time, I will try to make releases for other distros.

## Building the application

A Dockerfile is provided to ensure everyone has the same build and run environment. Note that there is no specific run image though. To build the application, follow the instructions below:

<ol start="1"> <li>Build the docker container, this shouldn't take long (30 minutes or so), since we use precompiled QT libraries. In any case, this only needs to be done once.</li>

```bash
docker build . \
  --build-arg USERNAME=$(id -un) \
  -f docker/maki_humble \
  -t maki:v1.0.0
```

Optionally, the docker image can be pulled directly from Github with:

```bash
docker pull ghcr.io/felipeacxavier/maki/dev:qt6.8.3
```
</ol>

<ol start="2"> <li>Run the docker image.</li>

```bash
docker run -it --rm \
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

Note that `~/ros2_ws` is an example and should be set to your local ROS workspace.
</ol>

<ol start="3"> <li>Inside the docker, we can now build Maki. There are two options available</li>

```bash
./scripts/linux/build.sh
```

  - If you need support for clangd and `compile_commands.json`, you can pass the options `--local-qt` and `--local-project`, which will make sure the generated `compile_commands.json` points to your local QT installation and project folder. For example:
```bash
./scripts/linux/build.sh --local-qt /opt/qt/6.8.3/gcc_64 --local-project /home/foo/maki
```
</ol>

<ol start="4"> <li>After building, it is possible to run the application with:</li>

```bash
./build/linux/debug/app/maki
```
</ol>

## Release version

<ol start="1"> <li>To create a release version, run the command</li>

```bash
./scripts/linux/build.sh --release
```
</ol>

<ol start="2"> <li>After building, it is possible to run the application with:</li>

```bash
./release/linux/bin/maki
```
</ol>

## Documentation

<ol start="1"> <li>To build the documentation, run:</li>

```bash
./scripts/linux/build.sh --docs
```
</ol>

<ol start="2"> <li>After building, it is possible to test it locally with:</li>

```bash
python3 -m http.server -d ./build/linux/debug/docs/html/
```
</ol>

## Other options

<ol start="1"> <li>There are multiple targets available. To list them, run:</li>

```bash
./scripts/linux/build.sh --list
```
</ol>
<ol start="2"> <li>Any of the listed options can then be run with:</li>

```bash
./scripts/linux/build.sh --target <option name>
```
</ol>
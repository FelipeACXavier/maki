@page building_linux Building Linux

[TOC]

> [!IMPORTANT]
> The Linux Docker workflow has been used on Ubuntu 24.04. Other distributions and display-server configurations have not been fully validated.
>
> First complete the shared prerequisites in @ref building "Building MAKI": clone the repository and run _git submodule update --init --recursive_. The Linux build script will ask you to confirm these steps before building.

## Prerequisites

- **Git** and **Docker** installed on the host.
- A Linux host with a graphical session if you intend to run the MAKI GUI.
- Access to the Docker daemon and an internet connection to build or download the development image.
- An initialized MAKI checkout (including submodules) as described in @ref building.
- A ROS workspace if you want to mount one; the example below uses `~/ros2_ws`.

The supplied development container provides a consistent build environment, including the Qt libraries. You do not need to install Qt or ROS on the host to compile MAKI inside that container.

> [!NOTE]
> The shared @ref building page notes that MAKI does not install ROS packages when built outside Docker. For native builds, you will need to set up ROS and the other build dependencies yourself.

## Building

Open a terminal in the **MAKI repository root** before running the following commands.

### 1. Build or download the development container

Build the Docker image locally (typically needed only once, or when the Dockerfile changes):

```bash
docker build . \
  --build-arg USERNAME="$(id -un)" \
  -f docker/maki_humble \
  -t maki:v1.0.0
```

Alternatively, download the published development image:

```bash
docker pull ghcr.io/felipeacxavier/maki/dev:qt6.8.3
```

If you use the downloaded image, replace `maki:v1.0.0` with `ghcr.io/felipeacxavier/maki/dev:qt6.8.3` in the next command. Confirm that the tag matches the Qt version defined in `.qt-version`.

### 2. Start the container

The example below mounts the repository and a local ROS 2 workspace, and uses X11 forwarding for the graphical interface:

```bash
docker run -it --rm \
  --name maki \
  --user "$(id -u):$(id -g)" \
  --net=host \
  -e "DISPLAY=${DISPLAY:-:0}" \
  -e QT_X11_NO_MITSHM=1 \
  --device /dev/dri \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  -v "$PWD:/home/$(id -un)/maki:rw" \
  -v "$HOME/ros2_ws:/home/$(id -un)/ros2_ws:rw" \
  maki:v1.0.0
```

Adjust the ROS workspace mount to match your machine, or omit it if not needed. The image must have a corresponding user and home directory for this mount layout. X11 access controls, GPU permissions, and Wayland/XWayland setups may require host-specific adjustments.

### 3. Build MAKI

Inside the container, navigate to the mounted MAKI repository if necessary and run:

```bash
./scripts/linux/build.sh
```

The script first asks whether you have already completed the basic steps in `building.md` (cloning the repository and initializing submodules). Answer `y` to continue or `n` to stop and complete those steps.

For automated builds, **after completing those prerequisites**, use `--yes` to skip the interactive question:

```bash
./scripts/linux/build.sh --yes
```

By default, this creates a Debug build in `build/linux/debug`.

> [!TIP]
> For local clangd support, you can pass paths used to remap `compile_commands.json` to your host's project and Qt installation:
> ```bash
> ./scripts/linux/build.sh \
>   --local-qt /opt/qt/6.8.3/gcc_64 \
>   --local-project /home/foo/maki
> ```
> Replace these example paths with the paths on your machine.

### 4. Run the application

After a successful Debug build:

```bash
./build/linux/debug/app/maki
```

Running the GUI from a container requires a working graphical connection to the host.

## Release version

To build and deploy the Release configuration:

```bash
./scripts/linux/build.sh --release
```

The default Release command builds the `deploy-linux` target, installing to `release/linux`:

```bash
./release/linux/bin/maki
```

## Documentation

To build the documentation:

```bash
./scripts/linux/build.sh --docs
```

You can serve the generated HTML locally with Python:

```bash
python3 -m http.server -d ./build/linux/debug/docs/html/
```

Open `http://localhost:8000` in your browser. If you run the web server inside the container with `--net=host`, it should be reachable from the host at that address.

## Other build options

```bash
# Show usage and available flags
./scripts/linux/build.sh --help

# Build a specific target
./scripts/linux/build.sh --target kodagen

# List targets after configuring the build
./scripts/linux/build.sh --list

# Remove the Debug build directory and the Linux release directory
./scripts/linux/build.sh --clean

# Remove the Release build directory and the Linux release directory
./scripts/linux/build.sh --clean --release

# Use a different Qt prefix
./scripts/linux/build.sh --prefix /path/to/Qt/gcc_64

# Pass additional CMake cache entries
./scripts/linux/build.sh -- -DBUILD_TESTING=OFF
```

## Troubleshooting

- **Prerequisites prompt stops the build:** Follow @ref building, particularly `git submodule update --init --recursive`, and rerun the script. Use `--yes` only when those steps have already been completed.
- **Non-interactive build fails before CMake:** Pass `--yes` in CI or other non-interactive environments.
- **Docker cannot access the GUI:** Check `DISPLAY`, X11/XWayland access, and Docker's access to the X11 socket. The sample command is not a universal Wayland configuration.
- **Wrong Qt version or missing Qt:** Compare `.qt-version` with the Qt installation inside the container. Rebuild or update the image if needed.
- **CMake cache problems:** Run `./scripts/linux/build.sh --clean`, then rebuild. Note that `--clean` also removes `release/linux`.

# MAKI

MAKI is a low-code platform that targets robotic systems. It is build on top of KODA and aims to simplify the creation of formally verified ROS2 systems. MAKI is still in development and there is no easy way to install it yet

## Installation

A Dockerfile is provided to ensure everyone has the same build and run environment. Note that there is no specific run image though. To build the application, follow the instructions below:

  1. Clone this repository and move into it

  ```bash
  git clone https://github.com/FelipeACXavier/MAKI.git && cd MAKI
  ```

  2. Build the docker container, this might take some time (30+ minutes) since we need to build the QT library. Still, that only needs to be done once.

  ```bash
  docker build -f docker/Dockerfile -t maki:v1.0.0 .
  ```

  3. Run the docker image

  - Windows:

  ```powershell
  TODO: No idea to be honest
  ```

  - Linux, using X11

  ```bash
  docker run -it \
    --name maki \
    --user 1000:1000 \
    --net=host \
    -e DISPLAY=:0 \
    -e QT_X11_NO_MITSHM=1 \
    --device /dev/dri \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v .:/home/ubuntu/MAKI:rw \
    maki:v1.0.0
  ```
  4. Inside the docker, we can now build Maki. There are two options available

  - Windows: 

  ```powershell
  cmake -S . -B build-windows \
    -DDEPLOY_TARGET=windows \
    -DCMAKE_PREFIX_PATH=$HOME/Qt6-Windows \
    -DCMAKE_INSTALL_PREFIX=/home/ubuntu/MAKI/dist-windows \
    -DCMAKE_TOOLCHAIN_FILE=$HOME/cmake/toolchain-mingw64.cmake

  cmake --build build-windows -j 4
  ```

  - Linux (we have only tested in Ubuntu 24.04)

  ```bash
  cmake -S . -B build-linux \
    -DDEPLOY_TARGET=linux \
    -DCMAKE_PREFIX_PATH=$HOME/Qt6-Linux \
    -DCMAKE_INSTALL_PREFIX=/home/ubuntu/MAKI/dist-linux

  cmake --build build-linux -j 4
  ```
  5. Finally, to install the tool

  - Windows (This needs to be run in a Windows machine due to the need for `windeployqt.exe`)

  ```powershell
  cmake --build build-windows -j 4 --target deploy-windows
  ```

  - Linux (we have only tested in **Ubuntu 24.04**)

  ```bash
  cmake --build build-linux -j 4 --target deploy-linux
  ```

## Examples of KODA:

As mentioned, MAKI is build on top of KODA, a DSL for describing and composing robotic missions. For context, here, we present two small examples of this DSL. For more information, refer to the KODA repository.

The first example is a simple pick and place robot. It drives between two specified locations where it first picks an object and then places the object.

```
capability Drive(float x, float y) {
  action "/navigate_to_pose" "nav2_msgs::action::NavigateToPose::Goal" {
    trigger: void to_position(float x, float y);
    abort: void cancel();
    return: void in_position(float x, float y);
    error: void path_blocked();
  }
}

capability Vision() {
  action "/recognize" "std_msgs::String" {
    trigger: void recognize();
    return: void found();
    abort: void cancel();
  }
}

capability Grip(boolean grip) {
  action "/handler/action" "std_msgs::String" {
    trigger: void handle(boolean grip);
    return: void handled();
    error: void failed();
    abort: void cancel();
  }
}


task PickAndDrop (drive req Drive, vision req Vision, 
                  grip req Grip,
                  float x1, float y1, float x2, float y2) 
{
  trigger: void start(float x1, float y1, float x2, float y2);
  return: void done();
  abort: void abort();
  error: void failed();

  vars {
    float x1_ = x1 : 0.0
    float y1_ = y1 : 0.0
    float x2_ = x2 : 0.0
    float y2_ = y2 : 0.0
  }

  strategy {
    err: drive.abort() --> grip.abort() --> end;

    pick: (vision() on abort err) --> (grip(true) on error err on abort err);
    drop: grip(false) on error err on abort err;

    loop: (drive(x1_, y1_) on error err on abort err) -->
          pick -->
          (drive(x2_, y2_) on error err on abort err) -->
          drop;

    main: repeat(loop);
  }
}
```

The second example represents a small building automation scenario. The orchestrator monitors the occupancy of the room and set the lights accordingly, and it triggers the HVAC to control the CO2 levels.

```
capability Bridge(string zone) {
  service "/poll" ""{
    in: void poll(string scene);
  }
}

capability Co2Sensor() {    
  service "co2/normal" ""{
    out: void normal();
  }
  service "co2/high" ""{
    out: void high();
  }
}

capability EnvSensor() {
  service "environment/empty" "" {
    out: void empty();
  }
  service "environment/present" "" {
    out: void present();
  }
}

capability Lighting(string scene) {
  service "lighting/scene" "{ \"cmd:\" \"scene\" }" {
    in: void set(string scene);
  }
}

capability Ventilation(boolean up) {
    action "ventilation" "{ \"cmd:\" \"up\" }" {
      trigger: void solve(boolean up);
      return: void done();
      abort: void stop();
      error: void failed();
    }
}

task AirQuality (bridge req Bridge, sensor req Co2Sensor, environ req EnvSensor, 
                 light req Lighting, hvac req Ventilation, string zone)
{
  trigger: void start(string zone);
  return:  void done();
  abort:   void abort();
  error:   void failed();

  vars {
    string zone_ = zone : "zone1"
    int count_ = 0 : 0
  }

  strategy {
    err:  hvac.abort() --> light.set("Alert") --> end;
    abrt: hvac.abort() --> end;

    ventilation [mode]: within 300 do ( hvac(mode)
          on error err
          on abort abrt
          on environ.present() ( light.set("On") --> continue )
          on environ.empty() ( light.set("Off") --> continue )
        ) else err;

    main: every 10 { bridge.poll(zone_) }
      on abort ( abrt )
      on environ.present() ( light.set("On") )
      on environ.empty() ( light.set("Off") )
      on sensor.high() ( ventilation(true) )
      on sensor.normal() ( ventilation(false) );
  }
}
```

## Styling ideas

- Red Inferno: [\#bb2c1e](https://colorkit.co/color/bb2c1e/)
- Flame of Prometheus: [\#d73800](https://colorkit.co/color/d73800/)
- Freshly Baked: [\#e5c185](https://colorkit.co/color/e5c185/)
- Fondue: [\#fbf2c4](https://colorkit.co/color/fbf2c4/)
- Neptune Green: [\#7ebb9c](https://colorkit.co/color/7ebb9c/)
- Emperor Jade: [\#018373](https://colorkit.co/color/018373/)
- Windows 95 Desktop: [\#008585](https://colorkit.co/color/008585/)

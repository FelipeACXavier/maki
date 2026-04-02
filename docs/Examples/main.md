@page examples Examples

Here, we present a few examples of tasks defined with the KODA language and their corresponding MAKI files.

## Pick and place

A small task where a robot needs to continuously drive between two locations, to pick up and object and drop it off.

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


task PickAndDrop (drive req Drive, vision req Vision, grip req Grip, float x1, float y1, float x2, float y2) 
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
    err: vision.cancel() --> drive.cancel() --> grip.cancel() --> end;

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

With KODA, it becomes very easy to integrate additional features. For example, if the drive capability is imprecise and the robots needs to be able to properly dock, the additional capability can be added:

```
capability Docking(boolean approach) {
  action "/dock" "std_msgs::String" {
    trigger: void start(boolean approach);
    abort: void cancel();
    return: void finished();
    error: void failed();
  }
}
```

And the strategy can be updated accordingly:
```
strategy {
    err: vision.cancel() --> drive.cancel() --> grip.cancel() --> end;

    pick: (vision() on abort err) --> (grip(true) on error err on abort err);
    drop: grip(false) on error err on abort err;

    // We add a new flow to make this behavior reusable
    drive_and_dock[x, y]: docking(false) -> drive(x, y) -> docking(true)

    // Now, we always try to undock and then dock before driving
    loop: (drive_and_dock(x1_, y1_) on error err on abort err) -->
          pick -->
          (drive_and_dock(x2_, y2_) on error err on abort err) -->
          drop;

    main: repeat(loop);
  }
```


## Simple smart building

In this example, we have a typical smart building setup. The goal is to maintain air quality and manage lighting in open-plan zones. The task polls the devices every few milliseconds and updates the services accordingly. Once a person is detected in the room, the lights turn on; they turn off once the room is empty. Moreover, while there are people in the room, the ventilation system is responsible for addressing air quality issues within a five-minute time frame.

```
capability Bridge(string zone) {
  service "poll" ""{
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

capability Light(string scene) {
  service "lighting/scene" "" {
    in: void set(string scene);
  }
}

capability Ventilation(string up) {
  action "ventilation" "" {
    trigger: void solve(string up);
    return: void done();
    abort: void stop();
    error: void failed();
  }
}

task AirQuality (bridge req Bridge, co2sensor req Co2Sensor, envsensor req EnvSensor, 
                 light req Light, vent req Ventilation, string zone)
{
  trigger: void start(string zone);
  return:  void done();
  abort:   void abort();
  error:   void failed();

  vars {
    string zone_ = zone : ""
  }

  strategy {
    err:  vent.stop() --> light.set("Alert") --> end;
    abrt: vent.stop() --> end;

    ventilation [mode]: within 300 do ( vent(mode)
          on error err
          on abort abrt
          on envsensor.present() ( light.set("On") --> continue )
          on envsensor.empty() ( light.set("Off") --> continue )
        ) else err;

    main: every 10 { bridge.poll(zone_) }
      on abort ( abrt )
      on envsensor.present() ( light.set("On") --> continue )
      on envsensor.empty() ( light.set("Off") --> continue )
      on co2sensor.high() ( ventilation("down") --> continue )
      on co2sensor.normal() ( ventilation("up") --> continue );
  }
}
```

## Quality check

In this example, we have a pick-and-place cell equipped with a conveyor belt, a robotic arm, a camera, and a ticketing system to notify of any failures. The cell begins by activating its inspection camera and conveyor. The conveyor runs until an invalid part is detected; then, a failure ticket is created, and the robot arm is sent to place the part in the rejected bin.

```
capability Conveyor() {
  action "conveyor" ""{
    trigger:  void move();
    abort:    void stop();
    return:   void done();
    error:    void failed();
  }
}

capability RobotArm(string pick) {
  action "arm" "" {
    trigger:  void act(string pick);
    abort:    void stop();
    return:   void done();
    error:    void failed();
  }
}

capability ImageInspector() {
  service "image/start" ""{
    in: void start();
  }
  service "image/stop" ""{
    in: void stop();
  }
  service "image/invalid" ""{
    out: void invalid();
  }
}

capability Ticket() {
  service "ticket/create" ""{
    in: void create();
  }
}

task QualityCheckCell(arm req RobotArm, conveyor req Conveyor, 
                      image req ImageInspector, ticket req Ticket) {
  trigger: void start();
  return:  void done();
  abort:   void abort();
  error:   void failed();

  strategy {
    abrt: conveyor.stop() --> image.stop() --> arm.stop() --> end;

    reject: ( arm("reject_location")
              on error abrt
              on abort abrt ) --> ticket.create();

    detected: conveyor.stop() -->
        ( arm("pick_location")
          on error abrt
          on abort abrt ) --> reject;

    main: repeat( image.start() -->
      ( conveyor()
        on error abrt
        on abort abrt
        on image.invalid() detected )
    );
  }
}
```
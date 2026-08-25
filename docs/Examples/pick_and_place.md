@page pick_and_place Pick and Place

This example shows a small task where a robot needs to continuously drive between two locations, to pick up and object and drop it off.

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
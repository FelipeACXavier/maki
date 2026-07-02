@page hospital_task Medicine delivery

As extension of the @ref pick_and_place, this example shows a medicine delivery task. 
We consider a robot whose task is to retrieve medication packages from a storage cabinet and deliver them to patient rooms.

The task is decomposed into several actions: navigating to the medicine cabinet, locating the package, grasping it, navigating to the destination, and delivering the package.
Throughout the duration of the task, the robot must monitor its battery level and charge in case the battery level goes below a certain threshold.
Although simplified, this scenario captures several capabilities commonly required in autonomous service robots, including navigation, object handling, and battery management.
Moreover, different behavioural patterns can be used to coordinate the actions in this task, which allows us to demonstrate the various language constructs discussed in the following sections.
With the running example introduced, we can move on to the explanation of the proposed approach.

```
task hospitalTask(drive req Drive, approach req Approach, grip req Grip, objdetection req ObjectDetection, batterymonitor req BatteryMonitor, charge req Charging)
{
  trigger: void start();
  abort: void cancel();
  return: void done();
  error: void failed();
  strategy {
    main: repeat(join(repeat(floop) | fcheck));
    fcheck: batterymonitor(30) on error (fstop)
        --> abort(floop)
        --> drive_to("charger_location")
        --> charge();
    floop: (fdrive_to("pick_location"))
        --> (fpickdrop("pick"))
        --> (fdrive_to("drop_location"))
        --> (fpickdrop("drop"));
    fdrive_to[location]: (drive(location)
                            on error (fstop)
                            on abort (fstop))
        --> approach(location, true);
    fpickdrop[pick]:
        (within 30 do (objdetection()) else (fstop))
            --> (grip(pick))
            --> (approach("", false));
    fstop: (drive.cancel())
        --> (approach.cancel())
        --> (arucodetection.cancel())
        --> (grip.cancel())
        --> (end);
  }
}
```
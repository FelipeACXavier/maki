@page quality_check_cell Quality check cell

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
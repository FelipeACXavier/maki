@page smart_room Smart room

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
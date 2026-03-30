import subprocess
import tempfile
import argparse
import random
import math

# Randomize the pose a little bit
x = random.uniform(0.2, 0.5)
y = random.uniform(3, 4)
direction = random.choice([-1, 1])
SPEED = 3
ROOM_WAIT = 10

POINTS = {
  "top_outside":    ( 0.0,  8.0, 1.0, 0.0, 0.0, direction * math.pi / 2),
  "bottom_outside": ( 0.0, -8.0, 1.0, 0.0, 0.0, direction * math.pi / 2),
  "top_inside":     ( 0.0,  5.0, 1.0, 0.0, 0.0, direction * math.pi / 2),
  "bottom_inside":  ( 0.0, -5.0, 1.0, 0.0, 0.0, direction * math.pi / 2),
  # Left
  "room0_outside":  (  -x, -4.0, 1.0, 0.0, 0.0,  math.pi),
  "room0_inside":   (-4.0, -4.0, 1.0, 0.0, 0.0,        0),
  "room0_outside_2":(  -x, -4.0, 1.0, 0.0, 0.0,        0),
  "room1_outside":  (  -x,  0.0, 1.0, 0.0, 0.0,  math.pi),
  "room1_inside":   (-4.0,  0.0, 1.0, 0.0, 0.0,        0),
  "room1_outside_2":(  -x,  0.0, 1.0, 0.0, 0.0,        0),
  "room2_outside":  (  -x,  4.0, 1.0, 0.0, 0.0,  math.pi),
  "room2_inside":   (-4.0,  4.0, 1.0, 0.0, 0.0,        0),
  "room2_outside_2":(  -x,  4.0, 1.0, 0.0, 0.0,        0),
  # Righ-
  "room3_outside":  (   x, -4.0, 1.0, 0.0, 0.0,        0),
  "room3_inside":   ( 4.0, -4.0, 1.0, 0.0, 0.0,  math.pi),
  "room3_outside_2":(   x, -4.0, 1.0, 0.0, 0.0,  math.pi),
  "room4_outside":  (   x,  0.0, 1.0, 0.0, 0.0,        0),
  "room4_inside":   ( 4.0,  0.0, 1.0, 0.0, 0.0,  math.pi),
  "room4_outside_2":(   x,  0.0, 1.0, 0.0, 0.0,  math.pi),
  "room5_outside":  (   x,  4.0, 1.0, 0.0, 0.0,        0),
  "room5_inside":   ( 4.0,  4.0, 1.0, 0.0, 0.0,  math.pi),
  "room5_outside_2":(   x,  4.0, 1.0, 0.0, 0.0,  math.pi),
}

def get_time(pa, pb):
  return (math.hypot(pa[0] - pb[0], pa[1] - pb[1]) / SPEED)


def point_to_sdf(key, previous, time):
  outside = POINTS[f'{key}_outside']
  outside_v = ""
  for p in outside:
    outside_v += f'{p} '

  inside = POINTS[f'{key}_inside']
  inside_v = ""
  for p in inside:
    inside_v += f'{p} '

  if previous is not None:
    time = time + get_time(previous, outside)

  if 'room' in key:
    outside_2 = POINTS[f'{key}_outside_2']
    outside_v_2 = ""
    for p in outside_2:
      outside_v_2 += f'{p} '

    # Takes 2 seconds to enter the room
    time0 = time + get_time(outside, inside) + ROOM_WAIT / 3
    time1 = time0 + get_time(inside, outside_2) + ROOM_WAIT * 2 / 3
    # Stays in the room for 8 seconds
    return f"""
      <waypoint><time>{time:.2f}</time><pose>{outside_v}</pose></waypoint>
      <waypoint><time>{time0:.2f}</time><pose>{inside_v}</pose></waypoint>
      <waypoint><time>{time1:.2f}</time><pose>{outside_v_2}</pose></waypoint>
    """, outside, time1
  else:
    # Takes 2 seconds to enter the building
    if previous is None:
      time0 = time + get_time(outside, inside)
      return f"""
        <waypoint><time>{time:.2f}</time><pose>{outside_v}</pose></waypoint>
        <waypoint><time>{time0:.2f}</time><pose>{inside_v}</pose></waypoint>
      """, inside, time0
    else:
      time0 = time + get_time(inside, outside)
      return f"""
        <waypoint><time>{time:.2f}</time><pose>{inside_v}</pose></waypoint>
        <waypoint><time>{time0:.2f}</time><pose>{outside_v}</pose></waypoint>
      """, outside, time0

def actor_sdf(name, loop, route):
    # Room variables
    trajectory = ""
    tmp, previous, time = point_to_sdf('top', None, 0) if direction > 1 else point_to_sdf('bottom', None, 0)
    trajectory += tmp

    for point in route:
      tmp, previous, time = point_to_sdf(point, previous, time)
      trajectory += tmp

    tmp, previous, time = point_to_sdf('bottom', previous, time) if direction > 1 else point_to_sdf('top', previous, time)
    trajectory += tmp

    return f"""
<sdf version="1.9">
  <actor name="{name}">
    <pose>0 0 1 0 0 0</pose>
    <skin>
      <filename>https://fuel.gazebosim.org/1.0/Mingfei/models/actor/tip/files/meshes/walk.dae</filename>
      <scale>1.0</scale>
    </skin>

    <animation name="walk">
      <filename>https://fuel.gazebosim.org/1.0/Mingfei/models/actor/tip/files/meshes/walk.dae</filename>
      <interpolate_x>true</interpolate_x>
    </animation>

    <script>
      {"<loop>true</loop>" if loop else ""}
      <delay_start>0.0</delay_start>
      <auto_start>true</auto_start>

      <trajectory id="0" type="walk" tension="1.0">
        {trajectory}
      </trajectory>
    </script>
  </actor>
</sdf>
"""

list_of_choices = ["room0", "loop", "passerby"]

def remove_entity(name):
    subprocess.run([
        "gz", "service",
        "-s", "/world/default/remove",
        "--reqtype", "gz.msgs.Entity",
        "--reptype", "gz.msgs.Boolean",
        "--timeout", "2000",
        "--req", f'name: "{name}" type: ACTOR'
    ], check=True)


def create_entity(actor_name, loop, route):
    with tempfile.NamedTemporaryFile(mode="w", suffix=".sdf", delete=False) as f:
        # if actor_type == "loop":
        f.write(actor_sdf(actor_name, loop, route))
        # elif actor_type == "passerby":
            # f.write(passerby_sdf(actor_name, loop))
        path = f.name

    print(f'Created actor file: {path}')
    subprocess.run([
        "ros2", "run", "ros_gz_sim", "create",
        "-world", "default",
        "-file", path,
    ], check=True)


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description="Control actors in the simulation")
    group = ap.add_mutually_exclusive_group()
    group.add_argument("-c", "--create", type=str, help="Create an entity")
    group.add_argument("-r", "--remove", type=str, help="Remove an entity")
    # ap.add_argument("-t", "--type", required=False, default="passerby", help="Type of the entity", choices=list_of_choices)
    ap.add_argument("-l", "--loop", required=False, action='store_true', help="Whether to loop the actor")
    ap.add_argument("--route", nargs="+", help="Sequence of rooms to visit, e.g. room0 room1 room0 room2")
    ap.add_argument("--speed", required=False, type=float, help="How fast the actor moves")
    ap.add_argument("--wait", required=False, type=float, help="How long to wait in the room")

    args = ap.parse_args()

    if args.speed:
      SPEED = args.speed
    if args.wait:
      ROOM_WAIT = args.wait

    if args.create is not None:
        create_entity(args.create, args.loop, args.route)
    elif args.remove is not None:
        remove_entity(args.remove)
    else:
        print("No create or remove option passed")

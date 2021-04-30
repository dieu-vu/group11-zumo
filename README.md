# Zumo Peoject
 ####Controlling Zumo Robot by using Metropolia PSoC-Zumo adapter

 ### Team member: ###
 [Dung Tran](https://github.com/pdung1989) and [Dieu Vu](https://github.com/dieu-vu)
 
 ## Projects ##
 
### 1. Sumo Wrestling ###
  
**Track**: sumo_ring_w_obstacle 

Robot must drive along the line to the edge of the sumo ring and wait for start signal. The start signal is given with an IR remote. The robot drives around and avoids hitting the red triangle. Robot uses reflectance sensors to stay inside the ring. After entering the ring robot must stop when user button is pressed.

Robot is considered to be out of the ring if the center of the robot crosses the line. Instructors will judge who is out and who is not.

**Start**: Robot drives to the edge of the ring and waits on the line for a start signal. The start signal is given with an IR remote.

Robot must send the following data over MQTT connection:

- Ready indicator when robot stops on the start line and starts to wait for IR start signal:
  - Subtopic: ready
  - Data: “zumo”
- Start time (systick based time stamp = number of milliseconds since the robot code started)
  - Subtopic: start
- Stop time (when the user button is pressed robot stops and sends stop time stamp)
  - Subtopic: stop
- Run time (from start to finish, number of milliseconds between start and stop)
  - Subtopic: time
- Timestamp, number of milliseconds since boot, of each turn to avoid the obstacle
  - Subtopic: obstacle

For example, Zumo028 sends:
```
Zumo028/ready zumo
Zumo028/start 2301
Zumo028/obstacle 3570
Zumo028/obstacle 4553
Zumo028/obstacle 4988
Zumo028/stop 5210
Zumo028/time 2909
```
### 2. Line following
**Track**: race_track

Robot follows the black line using reflective sensors in the front of the robot. Track total length is about 10 meters. Robot has six reflective sensors. The center sensors are used mainly to keep the robot on the track and side sensors to detect intersections or tight curves on the track. The robot must follow the start and stop rules stated below. The intersections are in a 90-degree angle on a straight line. The competition track is 2 cm wide. Make the robot run as fast as possible.

**Start**: Robot drives to the line and waits on the line for a start signal. The start signal is given with an IR remote.
**Finish**: Robot stops on the second line.
**Hint**: Implement line following first. When your robot is able to follow the line then implement start and stop logic.

Robot must send the following data over MQTT connection:
- Ready indicator when robot stops on the start line and starts to wait for IR start signal:
	- Subtopic: ready
	- Data: “line”
- Start time (systick based time stamp = number of milliseconds since the robot code started)
	- Subtopic: start
- Stop time (when the robot stops on the finish line it sends stop time stamp)
	- Subtopic: stop
- Run time (from start to finish, number of milliseconds between start and stop)
	- Subtopic: time
- Bonus (optional):
	- Timestamp if both centre sensors go off the track
		- Subtopic: miss
	- Timestamp when both centre sensors go back to the track after a miss
		- Subtopic: line
	- You must implement both timestamps to get a bonus

For example, Zumo028 sends:
```
Zumo028/ready line
Zumo028/start 1210
Zumo028/stop 54290
Zumo028/time 53080
```
```
Zumo028/ready line
Zumo028/start 1210
Zumo028/miss 7912
Zumo028/line 8012
Zumo028/miss 32103
Zumo028/line 32630
Zumo028/miss 53223
Zumo028/line 53933
Zumo028/stop 54290
Zumo028/time 53080
```

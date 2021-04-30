# Controlling Zumo Robot by using Metropolia PSoC-Zumo adapter
 ### Team member ###
 [Dung Tran](https://github.com/pdung1989) and  [Dieu Vu](https://github.com/dieu-vu)
 
 ### Projects ###
 
#### 1. Sumo Wrestling ####
  
##### Track: sumo_ring_w_obstacle #####

Robot must drive along the line to the edge of the sumo ring and wait for start signal. The start signal is given with an IR remote. The robot drives around and avoids hitting the red triangle. Robot uses reflectance sensors to stay inside the ring. After entering the ring robot must stop when user button is pressed.

Robot is considered to be out of the ring if the center of the robot crosses the line. Instructors will judge who is out and who is not.

Start: Robot drives to the edge of the ring and waits on the line for a start signal. The start signal is given with an IR remote.

Robot must send the following data over MQTT connection:

Ready indicator when robot stops on the start line and starts to wait for IR start signal:
Subtopic: ready
Data: “zumo”
Start time (systick based time stamp = number of milliseconds since the robot code started)
Subtopic: start
Stop time (when the user button is pressed robot stops and sends stop time stamp)
Subtopic: stop
Run time (from start to finish, number of milliseconds between start and stop)
Subtopic: time
Timestamp, number of milliseconds since boot, of each turn to avoid the obstacle
Subtopic: obstacle
For example, Zumo028 sends:

>>Zumo028/ready zumo
>>Zumo028/start 2301
>>Zumo028/obstacle 3570
>>Zumo028/obstacle 4553
>>Zumo028/obstacle 4988
>>Zumo028/stop 5210
>>Zumo028/time 2909

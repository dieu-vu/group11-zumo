/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <library.h>
#include <stdlib.h>
#include <time.h>
#define MAIN_TOPIC "Zumo011/"
#define READY_SUBTOPIC "ready"
#define START_SUBTOPIC "start"
#define STOP_SUBTOPIC "stop"
#define RUNTIME_SUBTOPIC "time"
#define MISS_SUBTOPIC "miss"
#define LINE_SUBTOPIC "line"
#define POSITION_SUBTOPIC "position"
#define OBSTACLE_SUBTOPIC "obstacle"
enum direction {north = 0, east = 1, south = 2, west =3};


/***Start the program***/
void progStart(bool motor, bool reflectance, bool IR, bool Ultra){
    if(motor){
        //Enable motor controller
        motor_start();
        //Set speed to zero to stop motors
        motor_forward(0,0);
    }  
    if(reflectance){
        //Start reflectance sensor
        reflectance_start();
        //Set threshhold value to swith digi value between 0 and 1
        reflectance_set_threshold(11000,11000, 9000,9000, 11000, 11000);
    }
    if(IR){
        //Start the IR sensor
        IR_Start();
        //Clear IR receive buffer
        IR_flush(); 
    }
    if(Ultra){
        //Start the Ultrasonic distance sensor
        Ultra_Start();
     }
}

/***End the Program***/
void progEnd(uint32_t delay){
    while(true){
        vTaskDelay(delay);
    }
}

/***Tank turn with given direction***/
void tank_turn_direction(char dir, uint8_t speed, uint32_t delay){
    if (dir=='L'){
        SetMotors(1,0,speed, speed, delay);
    }
    else if (dir=='R'){
        SetMotors(0,1,speed, speed, delay);
    }
}

/***Motor turns to the given direction and correct the turn angle continuously using sensors***/
void motor_turn_to_direction(char dir){ 
    int moving_speed = 40;

    struct sensors_ dig;
    tank_turn_direction(dir, moving_speed, 20);
    vTaskDelay(1000);
    while (true){
        // turn until sensor has value 001100
        reflectance_digital(&dig);
        if (dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0) {
            break;            
        }
        if (dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 0 && dig.R1 == 0 && dig.R2 == 0 && dig.R3 == 0) {
            tank_turn_direction(dir, 8, 20);
        } else if (dig.L3 == 1) {  // turn left if sensor L3 in black
            tank_turn_direction('L', 10, 20);
        } else if (dig.L2 == 1) {   // turn left if sensor L2 in black
            tank_turn_direction('L', 8, 20);
        } else if (dig.R3 == 1) {       // turn right if sensor R3 in black
            tank_turn_direction('R', 10, 20);
        } else if (dig.R2 == 1) {       // turn right if sensor R2 in black
            tank_turn_direction('R', 8, 20);
        }
        motor_forward(0,0);
    }
}

/***Turn left from current direction and update the direction after the turn***/
void motor_turn_left(int *direction){ 
    motor_turn_to_direction('L');   //motor turns left
    if (*direction == north){
        *direction = west;          //then update direction to heading to the left if the previous direction is upward (north)
    } else{
        *direction = *direction - 1; //otherwise following the sequence in the enumeration
    }   
}

/***Turn right from current direction and update the direction after the turn***/
void motor_turn_right(int *direction){
    motor_turn_to_direction('R');
    if (*direction == west) {
        *direction = north;
    } else {
        *direction = *direction + 1;
    } 
}

/***Check if seeing a line, Motor stops***/
void detect_horizontal_line(){  
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    motor_forward(100,0);    
    
    while(!(dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // stop motors
}    


/***Line following***/
void follow_line(uint8 line_number){ // follow the curve line and turn around to find the way when out of the track
    
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    uint8 count = 0;
    uint8 count_miss = 0;
    uint16 missed = 0;
    uint32_t start_time = 0;
    uint32_t stop_time = 0;
    uint32_t miss_time = 0;
    uint32_t line_time = 0;
    bool in_line = true;
    
    /*Start moving until meeting the first line:*/
    detect_horizontal_line();

    while (count<line_number){ 
        /*Allow IR signal sent to continue follow the curve after seeing a full line, 
        if the number of lines robot has met is smaller than line_number given.*/
        reflectance_digital(&dig);
           
        /*if detecting a full line:*/
        if(dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
            if (!in_line){
                count++;
                in_line=true;
            }
            if (count == 0){  //Only stop to receive the IR signal at the first and the last line number
                motor_forward(0,0);
                print_mqtt(MAIN_TOPIC,"%s line", READY_SUBTOPIC);
                IR_wait();                  //wait for signal
                start_time = xTaskGetTickCount(); // get start time
                print_mqtt(MAIN_TOPIC,"%s %d", START_SUBTOPIC, start_time); // print out start time MQTT message                
            }   
            if (count == line_number){   //when robot is at the last crossing line
                motor_forward(0,0);      //it stops
                stop_time = xTaskGetTickCount(); // then gets stop time
                print_mqtt(MAIN_TOPIC,"%s %d", STOP_SUBTOPIC, stop_time);
                print_mqtt(MAIN_TOPIC,"%s %d", RUNTIME_SUBTOPIC, stop_time - start_time); //return runtime
                print_mqtt(MAIN_TOPIC,"missed: %d times", missed);
                IR_wait();                  //wait for signal
            }
            //After detecting one full crossing line, keep moving to pass the line as long as count< line_number
            while (dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
                motor_forward(50,0); 
                reflectance_digital(&dig);     
            }       
        }
        else{
            in_line = false;
            if (dig.L1 == 1 || dig.R1 == 1){
                //move forward if L1 and R1 have sensoring signal
                if (count_miss != 0){ //If the robot has missed the line in the last iteration and then come back to line
                    line_time = xTaskGetTickCount();
                    print_mqtt(MAIN_TOPIC,"%s %d", LINE_SUBTOPIC, line_time); //print "get back to line" message
                    count_miss = 0; //then reset the missing line counter
                }
                motor_forward(250,0);
            }
            else if (dig.L3 ==1 || dig.L2 == 1){
                //turn left when there is signal on the left
                motor_turn(10,250,40);
                motor_forward(100,0);     
            } 
            else if (dig.R3 ==1 || dig.R2 ==1){
                //turn right when there is signal on the right
                motor_turn(250,10,40);
                motor_forward(100,0);               
            } 
            else if (!(dig.L3 == 1 && dig.L2 == 1 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 1 && dig.R3 == 1)){ 
                //if no signal found, turn clockwise and move a bit until robot can find the way again 
                tank_turn_direction('R',75,50);
                motor_forward(75,50);    
            }  
            else {
                motor_forward(250,0); //Otherwise, the robot moving forward
            }    
        }
        reflectance_digital(&dig);
        if (!(dig.L1 == 1 || dig.R1 == 1)){ //If the center sensors BOTH go off the line
            while (count_miss <1){
                miss_time = xTaskGetTickCount();
                print_mqtt(MAIN_TOPIC,"%s %d", MISS_SUBTOPIC, miss_time); //print missing message
                count_miss++;
                missed++;
            }
        }
    }
}

/***Function to check if robot pass (an) intersection(s)***/
void pass_intersection(uint8 intersect_count){     
   
    uint8 passed_intersections = 0;
    
    bool pass_intersection = false;
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    
    while(true){         
        if (passed_intersections >= intersect_count && intersect_count !=0){    //Escape the function when the robot pass the given number of intersections
            break;
        }
        reflectance_digital(&dig);  // read sensors
        // if all of sensors are detected, the robot is passing a crossing line
        while ((dig.L3 == 1 && dig.L2 == 1 && dig.L1 == 1 && dig.R1 == 1) || (dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 1 && dig.R3 == 1)){ //See an intersection
            motor_forward(100,0);
            reflectance_digital(&dig);
            pass_intersection = true;       //the robot keeps moving until it passes an intersection
        }
        
        if (dig.L1 == 0 && dig.R1 == 0){ //After passing an intersection and going over the lines, check if the robot reaches the edge
            break;  //escape if out of the maze
        }
        go_straight();      //otherwise, the robot adjust the direction to the straight line and continue going following the straight line
        if (pass_intersection){
            passed_intersections++;         //After passing the intersection, the line and not out of maze, update number of intersections passed
            pass_intersection = false;      //then reset pass boolean to check the next intersection  
        }
        motor_forward(0,0);   //stop and move a bit forward
        motor_forward(100, 10);
    }
}

/***Function to Adjust robot to align with the straight line***/
void go_straight(){     
    struct sensors_ dig;
    int aligned_times = 0;
    while(aligned_times < 5) {  //check 5 times if the robot can read signal from L1 and R1 to make sure it aligns with the straight line
        reflectance_digital(&dig);  // read sensors
        if (dig.L1 == 1 && dig.R1 == 1) {
            motor_forward(30, 1);     // go forward if R1 or L1 sensor =1
            aligned_times++;
        } else if (dig.L3 == 1){  // turn left if sensor L3 in black
            tank_turn_direction('L', 8, 20);
            aligned_times = 0;            
        } else if (dig.L2 == 1){   // turn left if sensor L2 in black
            aligned_times = 0;            
            tank_turn_direction('L', 5, 20);
        } else if (dig.R3 == 1){       // turn right if sensor R3 in black
            aligned_times = 0;            
            tank_turn_direction('R', 8, 20);
        } else if (dig.R2 == 1){       // turn right if sensor R2 in black
            aligned_times = 0;            
            tank_turn_direction('R', 5, 20);
        }
    }
    motor_forward(0,0); //stop
}

/***Robot moves until reaching the edge of the maze (L1 and R1 = 0)***/
void go_until_the_end(){ 
    struct sensors_ dig;
    while(true) {
        reflectance_digital(&dig);
        if (dig.L1 == 0 && dig.R1 == 0) {    // if not the end of the grid
            break;
        }
        go_straight();          //keep moving
        motor_forward(100, 100);
    }
}

/***Update coordinates when passing an intersection***/
void move_one_step(int *longitude, int *latitude, int *direction){ 
    pass_intersection(1);
    motor_forward(100,100); 
    motor_forward(0,0); //stop
    switch (*direction){
        case north:                 
            *latitude = *latitude + 1; 
            break;
        case east:
            *longitude = *longitude + 1;
            break;
        case south:
            *latitude = *latitude - 1;
            break;
        case west:
            *longitude = *longitude - 1;
            break;
    }
}

/***Deciding turn direction based on current coordinates and direction***/
int decide_direction(int longitude, int direction){ 
    /* If robot is heading to the left or on the left side of the grid (negative longitude)
    , then return -1 - Robot will turn left in the next step.
    Otherwise, return 1. Robot will turn right in the next step */
    switch (direction){
        case north:
            if (longitude < 0){  //if heading up north and on the left side
                return 1;       //turn right
            } else {            //if heading up north and not on the left side 
                return -1;      //turn left
            }
        case east:              //if heading to the right side
            return -1;          //turn left to go upward (go north)
        case south:             //if heading downward
            if (longitude < 0){   //if on the left side, turn left
                return -1;
            } else {                // if heading south (upward) and not on the right side, 
                return 1;           //turn right
            }
        case west:          //if heading to the left side
            return 1;       //turn right to go upward (go north)
    };
    return 1;
}

/***Function: Avoid obstacles and turn back to maze when reaching the edge***/

/*Strategy for avoid_obstacles and solving maze:
- Robot Turns when 1- Detecting obstacles and then 2- Reaching the edge of the maze
- Use the function decide_direction to decide which direction to turn
- Update the current coordinates and direction
- After the turn, use the function go_straight to align the robot with the straight line.
- Continue moving along the straight line one by one grid and update coordinates, until reaching the next obstacle or the edge of maze
- When robot is at coordinates (0,11), it decides the turning direction based on current direction and turns
- Then it move to the end of the maze using go_until_the_end function and then stop. */

void avoid_obstacles(){
   
    uint8 d = Ultra_GetDistance();
    int turn_direction;
    struct sensors_ dig;
    
    pass_intersection(2); //Pass the first two intersections at the start point and point (0,0)
    motor_forward(0,0); 
        
    int longitude = 0;  //Set coordinates at (0,0)
    int latitude = 0;
    int direction = 0; //Set direction up North
    print_mqtt(MAIN_TOPIC,"%s %d %d", POSITION_SUBTOPIC, longitude, latitude);
    
    while(true){
        reflectance_digital(&dig);  // read sensors
        d = Ultra_GetDistance();
        
        //Check if there is obstacle, or out of maze (L1 and R1 =0)
        //Decide the direction to turn base on the longitude of the current position  
        
        // 1- Detect obstacle from the distance < 15 cm
        if (d < 15){ 
            motor_forward(0,0);
            printf("Obstacle detected, distance %d\n", d);
            turn_direction = decide_direction(longitude, direction);    //check the previous direction and longitude to decide next direction
            if (turn_direction == -1){
                motor_turn_left(&direction);   //turn following the decided direction and update direction of the robot
            } else {
                motor_turn_right(&direction);
            }
            motor_forward(0,0);
            print_mqtt(MAIN_TOPIC,"%s %d %d", POSITION_SUBTOPIC, longitude, latitude);
        }
        //2- if the robot goes out of the maze, it also turns following the same logic when meeting obstacles
        else if (dig.L1 == 0 && dig.R1 == 0){  
            turn_direction = decide_direction(longitude, direction);
            if (turn_direction == -1){
                motor_turn_left(&direction);
            } else {
                motor_turn_right(&direction);
            }
            motor_forward(0,0);
            print_mqtt(MAIN_TOPIC,"%s %d %d", POSITION_SUBTOPIC, longitude, latitude);
        }
        //3- If not detecting any obstacles, or out of maze:
        else{
            move_one_step(&longitude, &latitude, &direction); //continue moving to the next intersections and update coordinates
            if (longitude == 0 && latitude >= 11) { //if at the coordinates with longitude 0 and any latitude higher than 11 (the last two full rows)
                motor_forward(0,0);                
                if (direction == 1){             //if robot is heading to the right side, then turn left
                    motor_turn_left(&direction);
                } else if (direction == 3){          //if robot is heading to the left side, then turn right
                    motor_turn_right(&direction);
                }
                print_mqtt(MAIN_TOPIC,"%s %d %d", POSITION_SUBTOPIC, longitude, latitude);
                break;             
            }
            print_mqtt(MAIN_TOPIC,"%s %d %d", POSITION_SUBTOPIC, longitude, latitude);
        }
    }
    go_until_the_end();
}

/***Solve maze function, using the avoid_obstacles function***/
void solve_maze(){
    
    uint32 start_time = 0;
    uint32 stop_time = 0;
    
    Ultra_Start();                          // Ultra Sonic Start function
    
    struct sensors_ dig; 
    reflectance_digital(&dig);
    
    detect_horizontal_line();
    IR_wait();  //IR start
    print_mqtt(MAIN_TOPIC, "%s maze", READY_SUBTOPIC);
    start_time = xTaskGetTickCount();
    print_mqtt(MAIN_TOPIC, "%s %d", START_SUBTOPIC, start_time);
     
    avoid_obstacles();  //Avoid obstacles
   
    motor_forward(0,0);
    stop_time = xTaskGetTickCount();
    print_mqtt(MAIN_TOPIC,"%s %d",STOP_SUBTOPIC, stop_time);
    print_mqtt(MAIN_TOPIC, "%s %d", RUNTIME_SUBTOPIC, stop_time-start_time);
 
}

/***Sumo Wrestling***/
void sumo_wrestling(){
    
    //Declare variables
    struct sensors_ dig;
    int d = Ultra_GetDistance();
    int random_val;
    TickType_t start = 0;
    TickType_t stop = 0;
    
    //When button is pressed and light on
    button_start(true);
    reflectance_digital(&dig);
    
    //Start to run to the corner by turning left and then go forward
    start = xTaskGetTickCount();
    motor_turn(0,100,260);   
    motor_forward(50,0);
    
    detect_horizontal_line();   //Stop when seeing the line
    print_mqtt(MAIN_TOPIC, "%s zumo", READY_SUBTOPIC); 
    
    IR_wait();  //Wait for IR signal
    print_mqtt(MAIN_TOPIC, "%s %d", START_SUBTOPIC, start);
    
    //Enter the ring
    while(dig.R3==1 && dig.R2==1 && dig.R1==1 && dig.L3==1 && dig.L2==1 && dig.L1==1) {
        motor_forward(200,50);
        reflectance_digital(&dig);   
    }
    motor_forward(50,0);
    
    //When the robot enters the ring, it will detect the obstacle and black edges of the ring  
    while(SW1_Read() == 1){
        d = Ultra_GetDistance();
        reflectance_digital(&dig);
    
        //Detect obstacle from the distance <= 10 cm
        if (d <= 10 ){ 
            print_mqtt(MAIN_TOPIC, "%s %d", OBSTACLE_SUBTOPIC, xTaskGetTickCount());
            motor_forward(0,0); 
            vTaskDelay(200);
            
            random_val = rand() %2; //Set random value to turn
            //Turn 90 degrees on random direction
            if (random_val == 1){
                tank_turn_direction('L',100,260);   //Turn left
                motor_forward(50,0);
                reflectance_digital(&dig);
            } else {
                tank_turn_direction('R',100,260);   //Turn right
                motor_forward(50,0);
                reflectance_digital(&dig);
            }    
        }
        //When detect the black edges, make the tank_turn
        //Check the sensors L3, R3
        if(dig.L3 == 1 && dig.R3 == 0){
            motor_forward(0,0);
            tank_turn_direction('R',100,260); //Turn right
            reflectance_digital(&dig);

        }else if(dig.R3 == 1 && dig.L3 == 0){
            motor_forward(0,0);
            tank_turn_direction('L',100,260);   //Turn left
            reflectance_digital(&dig);
        }
        //After turning from the edge, continue move forward
        motor_forward(100,50);
        reflectance_digital(&dig);
    }
    //Robot stop running when user button is pressed
    while(SW1_Read() == 1);
    motor_stop();
    stop = xTaskGetTickCount();
    print_mqtt(MAIN_TOPIC, "%s %d", STOP_SUBTOPIC, stop);
    print_mqtt(MAIN_TOPIC, "%s %d", RUNTIME_SUBTOPIC, stop - start);
}

/***Press button, light on***/
void button_start(bool button_press){
    
    if(button_press == 1){
    while(SW1_Read());  //When button is pressed
    BatteryLed_Write(true); //Turn on the led
    vTaskDelay(500);
    BatteryLed_Write(false);
    }
}

/***Set reflectance sensor for Line Follower project***/
void reflectance_linefollower(bool ref_line){
    
    if(ref_line == 1){
        struct sensors_ dig;
        reflectance_start();    //Start refluctance sensor
        reflectance_set_threshold(10000,10000,9000,9000,10000,10000); //Set threshhold value to swith digi value between 0 and 1
        reflectance_digital(&dig);
    }  
}

/***Set reflectance sensor for Maze Solving project***/
void reflectance_maze(bool ref_maze){
    
    if(ref_maze == 1){
        struct sensors_ dig;
        reflectance_start();    //Start refluctance sensor
        reflectance_set_threshold(9000, 9000, 14000, 14000, 9000, 9000); //Set threshhold value to swith digi value between 0 and 1
        reflectance_digital(&dig);
    }  
}
/* [] END OF FILE */
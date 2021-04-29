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

void motor_tank_turn() {
    int number = (rand() % (78 - 27 + 1)) + 27; // 27 equal 90 degree and 78 equal 270 degree
    SetMotors(0, 1, 100, 100, number * 10);
    motor_stop();
}

void progEnd(uint32_t delay){
//    bool led = false;
    while(true){
//        BatteryLed_Write(led^=1);
        vTaskDelay(delay);
    }
}

void tank_turn_direction(char dir, uint8_t speed, uint32_t delay){
    if (dir=='L'){
        SetMotors(1,0,speed, speed, delay);
    }
    else if (dir=='R'){
        SetMotors(0,1,speed, speed, delay);
    }
}

void motor_forward_line(uint8 number) {
    int lines = 0;
    bool in_line = true;
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    
    while(lines < number) {         // move if lines < number of lines
        reflectance_digital(&dig);  // read sensors
        
        // if all of sensors in line robot is in line and count the number of lines if in_line variable change from false
        if (dig.L3 == 1 && dig.L2 == 1 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 1 && dig.R3 == 1) {
            if (!in_line) {
                lines++;
                in_line = true;
            }
            motor_forward(100, 10);
        } else {
            in_line = false;    // robot isnot in line
            if (dig.L3 == 1) {  // turn left if sensor L3 in black
                motor_turn(100, 200, 10);
            } else if (dig.L2 == 1) {   // turn left if sensor L2 in black
                motor_turn(100, 150, 10);   
            } else if (dig.R3 == 1) {       // turn right if sensor R3 in black
                motor_turn(200, 100, 10);
            } else if (dig.R2 == 1) {       // turn right if sensor R2 in black
                motor_turn(150, 100, 10);
            } else {
                motor_forward(100, 10);     // go forward
            }
        }
    }
}

void motor_turn_to_direction(char dir) {
    int moving_speed = 40;

    struct sensors_ dig;
    tank_turn_direction(dir, moving_speed, 20);
    vTaskDelay(1000);
    while (true) {
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

void motor_turn_left(int *direction) {
    motor_turn_to_direction('L');
    if (*direction == 0) {
        *direction = 3;
    } else {
        *direction = *direction - 1;
    }   
}

void motor_turn_right(int *direction) {
    motor_turn_to_direction('R');
    if (*direction == 3) {
        *direction = 0;
    } else {
        *direction = *direction + 1;
    } 
}


void detect_horizontal_line(){ //Check if seeing a line, Motor stops 
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    motor_forward(100,0);    
    
    while(!(dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    motor_forward(0,0);         // stop motors
}    

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
            if (!in_line) {
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
            if (count == line_number){
                motor_forward(0,0);
                stop_time = xTaskGetTickCount(); // get start time
                print_mqtt(MAIN_TOPIC,"%s %d", STOP_SUBTOPIC, stop_time);
                print_mqtt(MAIN_TOPIC,"%s %d", RUNTIME_SUBTOPIC, stop_time - start_time);
                print_mqtt(MAIN_TOPIC,"missed: %d times", missed);
                IR_wait();                  //wait for signal
            }
            while (dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
                motor_forward(50,0);
                reflectance_digital(&dig);     
            }       
        }
        else {
            in_line = false;
            if (dig.L1 == 1 || dig.R1 == 1){
                //move forward if L1 and R1 have sensoring signal
                if (count_miss != 0){
                    line_time = xTaskGetTickCount();
                    print_mqtt(MAIN_TOPIC,"%s %d", LINE_SUBTOPIC, line_time);
                    count_miss = 0;
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
                //if no signal found, turn clockwise until robot can find the way again 
                tank_turn_direction('R',75,50);
                motor_forward(75,50);    
            }  
            else {
                motor_forward(250,0);
            }    
        }
        reflectance_digital(&dig);
        if (!(dig.L1 == 1 || dig.R1 == 1)){
            while (count_miss <1){
                miss_time = xTaskGetTickCount();
                print_mqtt(MAIN_TOPIC,"%s %d", MISS_SUBTOPIC, miss_time);
                count_miss++;
                missed++;
            }
        }
    }
    
}

void pass_intersection(uint16 time_limit, uint8 intersect_count) {
    uint16 current_time = 0;
    uint16 start_time = xTaskGetTickCount();
    uint8 passed_intersections = 0;
    
    bool pass_intersection = false;
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    
    
    while(true) {         
        //Check if the time since starting until current time is over time in
        current_time = xTaskGetTickCount();
        
        if (current_time - start_time > time_limit || (passed_intersections >= intersect_count && intersect_count !=0)){
            break;
        }
        reflectance_digital(&dig);  // read sensors
        // if all of sensors in line robot is in line and count the number of lines if in_line variable change from false
        while ((dig.L3 == 1 && dig.L2 == 1 && dig.L1 == 1 && dig.R1 == 1) || (dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 1 && dig.R3 == 1)) { //See an intersection
            motor_forward(100,0);
            reflectance_digital(&dig);
            pass_intersection = true;
        }
        
        if (dig.L1 == 0 && dig.R1 == 0) {
            break;
        }
        go_straight();
        if (pass_intersection) {
            passed_intersections++;
            start_time = xTaskGetTickCount();
            pass_intersection = false;        
        }
        motor_forward(0,0); //stop
        motor_forward(100, 10);
    }
}

void go_until_the_end(){
    struct sensors_ dig;
    while(true) {
        reflectance_digital(&dig);
        if (dig.L1 == 0 && dig.R1 == 0) {
            break;
        }
        go_straight();
        motor_forward(100, 100);
    }
}

void go_straight() {
    struct sensors_ dig;
    int aligned_times = 0;
    while(aligned_times < 5) {
        reflectance_digital(&dig);  // read sensors
        if (dig.L1 == 1 && dig.R1 == 1) {
            motor_forward(30, 1);     // go forward if R1 or L1 sensor =1
            aligned_times++;
        } else if (dig.L3 == 1) {  // turn left if sensor L3 in black
            tank_turn_direction('L', 8, 20);
            aligned_times = 0;            
        } else if (dig.L2 == 1) {   // turn left if sensor L2 in black
            aligned_times = 0;            
            tank_turn_direction('L', 5, 20);
        } else if (dig.R3 == 1) {       // turn right if sensor R3 in black
            aligned_times = 0;            
            tank_turn_direction('R', 8, 20);
        } else if (dig.R2 == 1) {       // turn right if sensor R2 in black
            aligned_times = 0;            
            tank_turn_direction('R', 5, 20);
        }
    }
    motor_forward(0,0); //stop
}

void move_one_step(int *longitude, int *latitude, int *direction) {
    pass_intersection(3500, 1);
    motor_forward(100,100); //stop
    motor_forward(0,0); //stop
    switch (*direction) {
        case 0:
            *latitude = *latitude + 1;
            break;
        case 1:
            *longitude = *longitude + 1;
            break;
        case 2:
            *latitude = *latitude - 1;
            break;
        case 3:
            *longitude = *longitude - 1;
            break;
    }
}

int decide_direction(int longitude, int direction){
    switch (direction) {
        case 0:
            if (longitude < 0) {
                return 1;
            } else {
                return -1;
            }
        case 1:
            return -1;
        case 2:
            if (longitude < 0) {
                return -1;
            } else {
                return 1;
            }
        case 3:
            return 1;
    };
    return 1;
}

void avoid_obstacles(){
   
    uint8 d = Ultra_GetDistance();
    int turn_direction;
    struct sensors_ dig;
    pass_intersection(5000, 2);
    motor_forward(0,0); //stop
        
    int longitude = 0;
    int latitude = 0;
    int direction = 0;
    
    while(true){
        reflectance_digital(&dig);  // read sensors
        d = Ultra_GetDistance();
        if (d < 15){ // Detect obstacle from the distance < 2 cm
            
            motor_forward(0,0); //stop
            printf("Obstacle detected, distance %d\n", d);
            turn_direction = decide_direction(longitude, direction);
            
            //turn 90 degrees on random direction
            if (turn_direction == -1){
                motor_turn_left(&direction);
            } else {
                motor_turn_right(&direction);
            }
            motor_forward(0,0);
        }
        else if (dig.L1 == 0 && dig.R1 == 0) {
            turn_direction = decide_direction(longitude, direction);

            if (turn_direction == -1){
                motor_turn_left(&direction);
            } else {
                motor_turn_right(&direction);
            }
        }
        else {
            move_one_step(&longitude, &latitude, &direction);
            if (longitude == 0 && latitude >= 11) {
                motor_forward(0,0);                
                if (direction == 1) {
                    motor_turn_left(&direction);
                } else if (direction == 3){ 
                    motor_turn_right(&direction);
                }
                break;             
            }
            printf("Direction: %d longitude: %d latitude: %d \n", direction, longitude, latitude);
        }
    }
    go_until_the_end();
}


void solve_maze() {
    
    uint16 start_time = 0;
    uint16 stop_time = 0;
    
    Ultra_Start();                          // Ultra Sonic Start function
    
    struct sensors_ dig; 
    reflectance_digital(&dig);
    
    detect_horizontal_line();
    IR_wait();
    print_mqtt(MAIN_TOPIC, "%s maze", READY_SUBTOPIC);
    start_time = xTaskGetTickCount();
    print_mqtt(MAIN_TOPIC, "%s %d", START_SUBTOPIC, start_time);
     
    avoid_obstacles();
   // pass_intersection(1400); // After 1400ms, if seeing no intersection, stop
    motor_forward(0,0);
    stop_time = xTaskGetTickCount();
    print_mqtt(MAIN_TOPIC,"%s %d",STOP_SUBTOPIC, stop_time);
    print_mqtt(MAIN_TOPIC, "%s %d", RUNTIME_SUBTOPIC, stop_time-start_time);

    
}
/* [] END OF FILE */

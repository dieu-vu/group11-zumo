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


void motor_tank_turn() {
    int number = (rand() % (78 - 27 + 1)) + 27; // 27 equal 90 degree and 78 equal 270 degree
    SetMotors(0, 1, 100, 100, number * 10);
    motor_stop();
    vTaskDelay(1000);
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
            } else if (dig.L2 == 1) {   // turn left if sensor L3 in black
                motor_turn(100, 150, 10);   
            } else if (dig.R3 == 1) {       // turn right if sensor L3 in black
                motor_turn(200, 100, 10);
            } else if (dig.R2 == 1) {       // turn right if sensor L3 in black
                motor_turn(150, 100, 10);
            } else {
                motor_forward(100, 10);     // go forward
            }
        }
    }
}

void motor_turn_left() {
    struct sensors_ dig;
    
    motor_forward(100, 30); 
    
    while (true) {      // turn left until sensor has value 001100
        reflectance_digital(&dig);
        
        motor_turn(0, 200, 1);
        if (dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0) {
            break;
        }
    }
}

void motor_turn_right() {
    struct sensors_ dig;
    
    motor_forward(100, 30);
    
    while (true) {      // turn right until sensor has value 001100
        reflectance_digital(&dig);
        
        motor_turn(200, 0, 1);
        if (dig.L3 == 0 && dig.L2 == 0 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 0 && dig.R3 == 0) {
            break;
        }
    }
}


void detect_horizontal_line(){ //Check if seeing a line, Motor stops 
    struct sensors_ dig; 
    
    reflectance_digital(&dig);

    vTaskDelay(100);
    motor_forward(30,0);    
    
    while(!(dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1)){
        reflectance_digital(&dig);
    }
    
    motor_forward(0,0);         // stop motors
    
    printf("Meet a bar! Stop and wait \n"); 

}    

void follow_curve(uint8 line_number){ // follow the curve line and turn around to find the way when out of the track
    
    struct sensors_ dig; 
    reflectance_digital(&dig);
    vTaskDelay(100);
    uint8 count = 0;
    bool in_line = true;
    
    //Start moving until meeting the first line:
    detect_horizontal_line();

    
    while (count<line_number){ 
        /*Allow IR signal sent to continue follow the curve after seeing a full line, 
        if the number of lines robot has met is smaller than line_number given.*/
        reflectance_digital(&dig);
        //if detecting a full line:
        if(dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
            motor_forward(0,0);
            IR_wait();                  //wait for signal
            printf("Reached the line, motor is waiting. Please send IR signal\n");
            if (!in_line) {
                count++;
                in_line=true;   
            }
            while (dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
                motor_forward(30,0);
                reflectance_digital(&dig);     
            }       
        }
        else {
            in_line = false;
            if (dig.L1 == 1 || dig.R1 == 1){
                //move forward if L1 and R1 have sensoring signal
                motor_forward(30,0);
            }    
            else if (dig.L3 ==1 || dig.L2 == 1){
                //turn left when there is signal on the left
                motor_turn(10,125,100);
                motor_forward(30,0);     
            } 
            else if (dig.R3 ==1 || dig.R2 ==1){
                //turn right when there is signal on the right
                motor_turn(125,10,100);
                motor_forward(30,0);               
            }
            else if (!(dig.L3 == 1 && dig.L2 == 1 && dig.L1 == 1 && dig.R1 == 1 && dig.R2 == 1 && dig.R3 == 1)){ 
                //if no signal found, turn clockwise until robot can find the way again 
                tank_turn_direction('R',75,0);
                motor_forward(30,0);    
            } 
            else {
                motor_forward(30,0);
            } 
        } 
    }
    
}

/* [] END OF FILE */

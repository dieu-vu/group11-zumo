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

/* [] END OF FILE */

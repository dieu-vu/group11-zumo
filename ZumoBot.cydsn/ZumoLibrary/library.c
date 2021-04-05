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


void motor_tank_turn();
void progEnd(uint32_t delay);
void tank_turn_direction(char dir, uint8_t speed, uint32_t delay);

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

/* [] END OF FILE */

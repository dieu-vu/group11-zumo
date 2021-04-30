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
#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include <stdint.h>
#include <time.h>


//Start the program
void progStart(bool motor, bool reflectance, bool IR, bool Ultra);

// End the program
void progEnd(uint32_t delay);

//Motor turn left or right
void tank_turn_direction(char dir, uint8_t speed, uint32_t delay);

//Motor turns to the given direction and correct the turn angle continuously using sensors
void motor_turn_to_direction(char dir);

//Go forward with number lines
void motor_forward_line (uint8 number);

// general function to turn
void motor_turn_to_direction(char dir);

//Turn left from current direction and update the direction after the turn
void motor_turn_left(int *direction);

//Turn right from current direction and update the direction after the turn
void motor_turn_right(int *direction);

//Motor stops and wait for IR signal
void detect_horizontal_line();

//Sumo wrestling
void sumo_wrestling();

//Function to check if robot pass (an) intersection(s)
void pass_intersection(uint8 intersect_count);

//Function to Adjust robot to align with the straight line
void go_straight();

//Robot moves until reaching the edge of the maze (L1 and R1 = 0)
void go_until_the_end();

//Update coordinates when passing an intersection
void move_one_step(int *longitude, int *latitude, int *direction);

//Deciding turn direction based on current coordinates and direction
int decide_direction(int longitude, int direction);

//Avoid obstacles
void avoid_obstacles();

//Solve maze
void solve_maze();





#endif
/* [] END OF FILE */


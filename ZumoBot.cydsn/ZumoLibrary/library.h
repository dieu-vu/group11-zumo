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

void go_straight();

// turn with random angle between 90 and 270 degrees
void motor_tank_turn();

// go forward with number lines
void motor_forward_line (uint8 number);

// general function to turn
void motor_turn_to_direction(char dir);
// turn left
void motor_turn_left();

// turn right
void motor_turn_right();

//Motor stops and wait for IR signal
void detect_horizontal_line();
void follow_line(uint8 line_number);

void pass_intersection(uint16 time_limit, uint8 intersect_count);
void avoid_obstacles();
void solve_maze();

void progEnd(uint32_t delay);
void tank_turn_direction(char dir, uint8_t speed, uint32_t delay);

#endif
/* [] END OF FILE */


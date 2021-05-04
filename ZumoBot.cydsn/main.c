/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
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
#include <stdlib.h>
#include <time.h>
#include "library.h"
/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

/******* ZUMO PROJECT GROUP 11 *******/

/*****SUMO WRESTLING*****/
#if 0
void zmain(void){
    
    progStart(true, true, true, true); //Start the program
    sumo_wrestling();   //Sumo wrestling
    progEnd(200);   //End the program
}
#endif

/*****LINE FOLLOWER*****/
#if 0
void zmain(void){
    
    progStart(true, false, true, false); //Start the program
    reflectance_linefollower(true); //Start refluctance sensors
    button_start(true); //Press the button
    follow_line(2); //Follow the curve line and stop at the 2nd horizontal line
    motor_stop();   //Disable motor controller
    progEnd(100);
}
#endif

/*****MAZE SOLVING*****/

#if 0
void zmain(void){
    
    progStart(true, false, true, false);    //Start the program
    reflectance_maze(true); //Start refluctance sensor
    button_start(true);  //Press the button
    solve_maze();   //Solve maze
    progEnd(200);
}   
#endif

/* [] END OF FILE */

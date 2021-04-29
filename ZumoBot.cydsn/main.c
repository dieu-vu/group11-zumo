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
#define MAIN_TOPIC "Zumo011/"


//#define TIME_TOPIC "button" //remove this in the main project code to submit
//#define TURN_TOPIC "Zumo11/turn" //remove this in the main project code to submit
//#define LAP_TOPIC "Zumo11/lap" //remove this in the main project code to submit

#define PRESSED 1
#define RELEASE 0



/******* PROJECT CODE FOLLOW *******/

/*****SUMO WRESTLING*****/
#if 0
//motor
void zmain(void){
    
    //Start the program
    progStart(true, true, true, true);
    //Sumo wrestling
    sumo_wrestling();
    //End the program
    progEnd(200);
}
#endif

/*****LINE FOLLOWER*****/
#if 0
//motor
void zmain(void){
    
    struct sensors_ dig;
    
    reflectance_start();
    
    reflectance_set_threshold(10000,10000,9000,9000,10000,10000); // set threshhold value to swith digi value between 0 and 1
    IR_Start();
    
    IR_flush(); // clear IR receive buffer
    //printf("Buffer cleared\n");
    
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    
    while(SW1_Read());
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    reflectance_digital(&dig);
    
   // follow the curve line and turn around to find the way when out of the track
    follow_line(2); //if passing param line_number>1, the robot turns around line_number times and continue until it meets the next line
    
    motor_stop();               // disable motor controller
    
    progEnd(100);
}
#endif

/*****MAZE SOLVING*****/

#if 1
//reflectance
void zmain(void){
    struct sensors_ dig;
    
    reflectance_start();    // start reflectance
    reflectance_set_threshold(9000, 9000, 14000, 14000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    IR_Start(); // start IR
    IR_flush(); // clear IR receive buffer
    motor_start(); // start motor
    motor_forward(0,0);   
    
    while(SW1_Read());  // wait SW1
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    reflectance_digital(&dig);

    solve_maze();
    
    progEnd(200);
}   
#endif




/***********************************/



/****WEEK 5 EX.1 ****/
#if 0
//motor
void zmain(void) {
    
    int press_1 = 0;
    int press_2 = 0;
    
   //button is pressed on the first time
    while(SW1_Read() == PRESSED);
    press_1 = xTaskGetTickCount();
    
    while(true) {
        while(SW1_Read() == PRESSED);
        press_2 = xTaskGetTickCount();
        int interval = (int)press_2 - (int)press_1;
        printf("\npress_1: %ds, press_2: %dms\n", press_1, press_2);
        printf("\nTime interval between two buttons pressed %dms\n", interval);
        print_mqtt(TIME_TOPIC,"%dms", interval);
        //button is released
        while(SW1_Read() == RELEASE);
        //set the first prssed value to the next one
        press_1 = press_2;
    }
    progEnd(100);
}
#endif


/****WEEK 5 EX.2 ****/
#if 0
//ultrasonic sensor - detect obstacles -random turn and record direction//
void zmain(void)
{
    Ultra_Start();                          // Ultra Sonic Start function
    while(SW1_Read());
    vTaskDelay(500);
    int d = Ultra_GetDistance();
    motor_start();
    
    int random_val;
    
    while(true) {
        
        d = Ultra_GetDistance();
        
        if (d <= 10){ // Detect obstacle from the distance < 10 cm
            motor_forward(0,0); //stop
            vTaskDelay(200);
            printf("Obstacle detected, stopped\n");
            
            motor_turn(0,100,1050);     //reverse 
            printf("Reversed\n");
            motor_forward(0,0);
            vTaskDelay(2000);
            
            random_val = rand() %2;
            
            //turn 90 degrees on random direction
            if (random_val == 1){
                print_mqtt(TURN_TOPIC, "LEFT"); // send the turn direction to topic Zumo11/turn
                tank_turn_direction('L',100,260);
                
            } else {
                print_mqtt(TURN_TOPIC, "RIGHT");
                tank_turn_direction('R',100,260);
            }    
            motor_forward(0,0);
            vTaskDelay(2000);
            motor_forward(30,0);  
            
        }
        motor_forward(60,0); 
    }
    
    progEnd(500);
    
}   
#endif

/****WEEK 5 EX.3 ****/

#if 0
//motor
void zmain(void)
{
    struct sensors_ dig;
    int count = 0;
    reflectance_start();
    bool in_line = false;
    uint16_t ir_command_time=0;
    uint16_t stop_time=0;
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    IR_Start();
    IR_flush(); //Clear IR receive buffer
    
    motor_start();
    motor_forward(0,0);  //set speed to zero
    
    while(SW1_Read());      //press user button
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    
    motor_forward(30,0);
    
   
    while (true) {
        reflectance_digital(&dig);
        
        //Go through the line
        if (dig.L3==1 && dig.L2 ==1 && dig.L1 ==1 && dig.R3==1 && dig.R2==1 && dig.R1==1) {
            motor_forward(0,0);
            stop_time=xTaskGetTickCount();
            printf("Stopping at line number %d\n", count+1);

            print_mqtt(LAP_TOPIC, "Elapsed time: %d ms", stop_time - ir_command_time);
            
            printf("Reached the line, motor is waiting. Please send IR signal\n");
            IR_wait();                  //wait for signal
            ir_command_time = xTaskGetTickCount(); //get time after IR command
            printf("IR command received\n");
            
            if (!in_line) {
                count++;
                in_line=true;   
            }
            while (dig.L1 == 1 && dig.L2 == 1 && dig.L3 == 1 && dig.R2 == 1 && dig.R1 == 1 &&dig.R3 == 1){
                motor_forward(30,0);
                reflectance_digital(&dig);     
            }
            motor_forward(0,0);
            vTaskDelay(100);
            in_line= false;
            printf("\nGot out of line number %d and moving on \n********\n",count);
            motor_forward(30,0);
            reflectance_digital(&dig);
            
        }
        else{
            in_line=false;
            motor_forward(30,0);
        }    

    }
    //motor_stop();
    progEnd(100);
}
#endif

/*****WEEK 4 EX.1*****/

#if 0
//motor
void zmain(void)
{
    struct sensors_ dig;
    int count = 0;
    reflectance_start();
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    IR_Start();
    IR_flush(); //Clear IR receive buffer
    
    motor_start();
    motor_forward(0,0);  //set speed to zero
    
    while(SW1_Read());      //press user button
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    
   //Enter to loop of 5 lines
    while (count < 5) {
        reflectance_digital(&dig);
        
        //Go thruogh the line
        while(dig.L3==1 && dig.L2 ==1 && dig.L1 ==1 && dig.R3==1 && dig.R2==1 && dig.R1==1) {
            motor_forward(100, 10);
            reflectance_digital(&dig);
        }
        
        while(!(dig.L3==1 && dig.L2 ==1 && dig.L1 ==1 && dig.R3==1 && dig.R2==1 && dig.R1==1)) { 
            motor_forward(150,10);
            reflectance_digital(&dig);
        }
        motor_forward(0,0);   
        
        count++;
        printf("We on line %d\n", count);
        
        //Witing for the IR-signal
        if(count==1)
        {
            IR_flush();
            IR_wait();
        }
    }
    //motor_stop();
    progEnd(100);
}
#endif

/****WEEK 4 EX.2*****/

#if 0
//motor
void zmain(void)
{
    
    struct sensors_ dig;
    
    reflectance_start();
    
    reflectance_set_threshold(11000,11000, 9000,9000, 11000, 11000); // set threshhold value to swith digi value between 0 and 1
    IR_Start();
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    
    while(SW1_Read());
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    reflectance_digital(&dig);
    
    printf("Starting sensor \n");
    
    motor_forward(30,0);     // moving forward
  
   // follow the curve line and turn around to find the way when out of the track
    follow_line(2); //if passing param line_number>1, the robot turns around line_number times and continue until it meets the next line
    
    motor_stop();               // disable motor controller
    
    progEnd(100);
}
#endif

/*****WEEK 4 EX.3*****/
#if 0
//reflectance
void zmain(void)
{
    reflectance_start();    // start reflectance
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    IR_Start(); // start IR
    IR_flush(); // clear IR receive buffer
    motor_start(); // start motor
    
    while(SW1_Read());  // wait SW1
    BatteryLed_Write(true);
    vTaskDelay(500);
    BatteryLed_Write(false);
    
    IR_wait();      // wait IR
    
    
    motor_forward_line(1);  // go forward to first intersection
    motor_turn_left();      // turn left
    motor_forward_line(1);  // go forward to second intersection
    motor_turn_right();     // turn right
    motor_forward_line(1);  // go forward to third intersection
    motor_turn_right();     // turn right
    motor_forward_line(1);  // go forward to fourth intersection
    motor_stop();           // stop motor
    
}   
#endif


/*****WEEK 3 EX.1*****/
#if 0
//motor
void zmain(void)
{
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors
    while(SW1_Read());
    BatteryLed_Write(true);
    vTaskDelay(1000);
    BatteryLed_Write(false);
    
    motor_forward(100,3000);           //moving forward
    motor_turn(210,105,500);
     
    motor_forward(100,1900);
    motor_turn(210,105,500);
    
    motor_forward(100,2400);
    motor_turn(200,50,500);
    
    motor_forward(100,1300);
    motor_turn(150,90,500);
    motor_forward(100,1100);
    
    motor_stop();             // disable motor controller
    
    while(true)
    {
        vTaskDelay(100);
    }
}
#endif


/*******WEEK 3 EX.2*******/

#if 0
//ultrasonic sensor - Avoiding obstacles//
void zmain(void)
{
    Ultra_Start();                          // Ultra Sonic Start function
    while(SW1_Read());
    vTaskDelay(500);
    int d = Ultra_GetDistance();
    motor_start();
    while(true) {
        
        // Print the detected distance (centimeters)
        d = Ultra_GetDistance();
        printf("distance = %d\r\n", d);
        if (d <= 10){
            printf("Oops, you are about to crash!\t");
            printf("distance = %d\r\n", d);
            motor_stop();
            motor_start();
            motor_backward(200,100);
            //turn 120 degrees
            motor_turn(0,250,100*2);
            motor_turn(150,250,100*2);
            //move forward and check distance again
            motor_forward(20,10);   
            d = Ultra_GetDistance(); //Check distance again after the turn
        } 
        printf("Now safe! distance = %d\r\n", d);
        motor_forward(80,0);
        
        vTaskDelay(200);
    }
    
}   
#endif

/*****WEEK 3 EX.3*****/
#if 0
//motor
void zmain(void)
{
    Ultra_Start();  // Ultra Sonic Start function
    while(SW1_Read());
    vTaskDelay(500);
    srand(time(NULL));
    int d = Ultra_GetDistance(); // detect original distance
    motor_start();
    while(true) {       
        // Print the detected distance (centimeters)
        d = Ultra_GetDistance();
        printf("distance = %d\r\n", d); 
        // if distance is closer than 10cm, reverses and turn
        if (d <= 10) {      
            motor_stop();
            vTaskDelay(1000);
            motor_start();
            motor_backward(200, 10);
            motor_stop();
            vTaskDelay(1000);
            motor_start();
            motor_tank_turn();  // turn 90 - 270 degrees
        }
        // robot go forward 10ms
        motor_start();
        motor_forward(100,10); 
        vTaskDelay(200);
    }
}
#endif

/**********************/

#if 0
// Hello World!
void zmain(void)
{
    printf("\nHello, World!\n");

    while(true)
    {
        vTaskDelay(100); // sleep (in an infinite loop)
    }
 }   
#endif

#if 0
// Name and age
void zmain(void)
{
    char name[32];
    int age;
    
    
    printf("\n\n");
    
    printf("Enter your name: ");
    //fflush(stdout);
    scanf("%s", name);
    printf("Enter your age: ");
    //fflush(stdout);
    scanf("%d", &age);
    
    printf("You are [%s], age = %d\n", name, age);

    while(true)
    {
        BatteryLed_Write(!SW1_Read());
        vTaskDelay(100);
    }
 }   
#endif


#if 0
//battery level//
void zmain(void)
{
    ADC_Battery_Start();        

    int16 adcresult =0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed

    while(true)
    {
        char msg[80];
        ADC_Battery_StartConvert(); // start sampling
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            
            // Print both ADC results and converted value
            printf("%d %f\r\n",adcresult, volts);
        }
        vTaskDelay(500);
    }
 }   
#endif

#if 0 

//Tick Timer Example
void zmain(void) 
{
	TickType_t Ttime = xTaskGetTickCount();
	TickType_t PreviousTtime = 0;

	while(true) 
	{
		while(SW1_Read()) vTaskDelay(1); // loop until user presses button
		Ttime = xTaskGetTickCount(); // take button press time
		/*Print out the time between button presses in seconds. int cast used to suppress warning messages*/
		printf("The amount of time between button presses is: %d.%d seconds\n", (int)(Ttime-PreviousTtime)/1000%60, (int)(Ttime-PreviousTtime)%1000);
		while(!SW1_Read())vTaskDelay(1); // loop while user is pressing the button
		PreviousTtime = Ttime; // remember previous press time
	}
	
}

#endif

#if 0
// button
void zmain(void)
{
    while(true) {
        printf("Press button within 5 seconds!\n");
	    TickType_t Ttime = xTaskGetTickCount(); // take start time
        bool timeout = false;
        while(SW1_Read() == 1) {
            if(xTaskGetTickCount() - Ttime > 5000U) { // too long time since start
                timeout = true;
                break;
            }
            vTaskDelay(10);
        }
        if(timeout) {
            printf("You didn't press the button\n");
        }
        else {
            printf("Good work\n");
            while(SW1_Read() == 0) vTaskDelay(10); // wait until button is released
        }
    }
}
#endif

#if 0
// button
void zmain(void)
{
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    while(true)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) vTaskDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor//
void zmain(void)
{
    Ultra_Start();                          // Ultra Sonic Start function
    
    while(true) {
        int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
        printf("distance = %d\r\n", d);
        vTaskDelay(200);
    }
}   
#endif

#if 0
//IR receiverm - how to wait for IR remote commands
void zmain(void)
{
    IR_Start();
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    bool led = false;
    // Toggle led when IR signal is received
    while(true)
    {
        IR_wait();  // wait for IR command
        led = !led;
        BatteryLed_Write(led);
        if(led) printf("Led is ON\n");
        else printf("Led is OFF\n");
    }    
 }   
#endif



#if 0
//IR receiver - read raw data
// RAW data is used when you know how your remote modulates data and you want to be able detect 
// which button was actually pressed. Typical remote control protocols requires a protocol specific
// state machine to decode button presses. Writing such a state machine is not trivial and requires
// that you have the specification of your remotes modulation and communication protocol    
void zmain(void)
{
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    // print received IR pulses and their lengths
    while(true)
    {
        if(IR_get(&IR_val, portMAX_DELAY)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
        }
    }    
 }   
#endif


#if 0
//reflectance
void zmain(void)
{
    struct sensors_ ref;
    struct sensors_ dig;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    

    while(true)
    {
        // read raw sensor values
        reflectance_read(&ref);
        // print out each period of reflectance sensors
        printf("%5d %5d %5d %5d %5d %5d\r\n", ref.L3, ref.L2, ref.L1, ref.R1, ref.R2, ref.R3);       
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig); 
        //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.L3, dig.L2, dig.L1, dig.R1, dig.R2, dig.R3);        
        
        vTaskDelay(200);
    }
}   
#endif


#if 0
//motor
void zmain(void)
{
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors

    vTaskDelay(3000);
    
    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // moving backward
     
    motor_forward(0,0);         // stop motors

    motor_stop();               // disable motor controller
    
    while(true)
    {
        vTaskDelay(100);
    }
}
#endif

#if 0
/* Example of how to use te Accelerometer!!!*/
void zmain(void)
{
    struct accData_ data;
    
    printf("Accelerometer test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Device Ok...\n");
    }
    
    while(true)
    {
        LSM303D_Read_Acc(&data);
        printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        vTaskDelay(50);
    }
 }   
#endif    

#if 0
// MQTT test
void zmain(void)
{
    int ctr = 0;

    printf("\nBoot\n");
    send_mqtt("Zumo01/debug", "Boot");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 

    while(true)
    {
        printf("Ctr: %d, Button: %d\n", ctr, SW1_Read());
        print_mqtt("Zumo01/debug", "Ctr: %d, Button: %d", ctr, SW1_Read());

        vTaskDelay(1000);
        ctr++;
    }
 }   
#endif


#if 0
void zmain(void)
{    
    struct accData_ data;
    struct sensors_ ref;
    struct sensors_ dig;
    
    printf("MQTT and sensor test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Accelerometer Ok...\n");
    }
    
    int ctr = 0;
    reflectance_start();
    while(true)
    {
        LSM303D_Read_Acc(&data);
        // send data when we detect a hit and at 10 second intervals
        if(data.accX > 1500 || ++ctr > 1000) {
            printf("Acc: %8d %8d %8d\n",data.accX, data.accY, data.accZ);
            print_mqtt("Zumo01/acc", "%d,%d,%d", data.accX, data.accY, data.accZ);
            reflectance_read(&ref);
            printf("Ref: %8d %8d %8d %8d %8d %8d\n", ref.L3, ref.L2, ref.L1, ref.R1, ref.R2, ref.R3);       
            print_mqtt("Zumo01/ref", "%d,%d,%d,%d,%d,%d", ref.L3, ref.L2, ref.L1, ref.R1, ref.R2, ref.R3);
            reflectance_digital(&dig);
            printf("Dig: %8d %8d %8d %8d %8d %8d\n", dig.L3, dig.L2, dig.L1, dig.R1, dig.R2, dig.R3);
            print_mqtt("Zumo01/dig", "%d,%d,%d,%d,%d,%d", dig.L3, dig.L2, dig.L1, dig.R1, dig.R2, dig.R3);
            ctr = 0;
        }
        vTaskDelay(10);
    }
 }   

#endif

#if 0
void zmain(void)
{    
    RTC_Start(); // start real time clock
    
    RTC_TIME_DATE now;

    // set current time
    now.Hour = 12;
    now.Min = 34;
    now.Sec = 56;
    now.DayOfMonth = 25;
    now.Month = 9;
    now.Year = 2018;
    RTC_WriteTime(&now); // write the time to real time clock

    while(true)
    {
        if(SW1_Read() == 0) {
            // read the current time
            RTC_DisableInt(); /* Disable Interrupt of RTC Component */
            now = *RTC_ReadTime(); /* copy the current time to a local variable */
            RTC_EnableInt(); /* Enable Interrupt of RTC Component */

            // print the current time
            printf("%2d:%02d.%02d\n", now.Hour, now.Min, now.Sec);
            
            // wait until button is released
            while(SW1_Read() == 0) vTaskDelay(50);
        }
        vTaskDelay(50);
    }
 }   
#endif

/* [] END OF FILE */

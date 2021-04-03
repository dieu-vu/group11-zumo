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
/* [] END OF FILE */

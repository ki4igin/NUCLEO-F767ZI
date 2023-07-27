#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f7xx.h"

#define MOTOR_USE_SERVO
// #define MOTOR_USE_MODBUS

void motor_init(void);
void motor_az_offset(int32_t offset_deg);
void motor_el_offset(int32_t offset_deg);

#endif

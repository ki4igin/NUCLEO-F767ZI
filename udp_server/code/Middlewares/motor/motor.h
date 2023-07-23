#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f7xx.h"

void motor_init(void);
void motor_az_offset(float delta);
void motor_el_offset(float delta);

#endif

#ifndef __SERVO_H__
#define __SERVO_H__

#include "stm32f7xx.h"

struct servo {
    struct tim {
        volatile uint32_t *const ccr;
        uint32_t ccr_val_min;
        uint32_t ccr_vals_range;
    } tim;

    int32_t offset_pos;
};

void servo_init(
    struct servo *s,
    uint32_t tim_arr_val,
    uint32_t pwm_width_min_us,
    uint32_t pwm_width_max_us);
void servo_moveto(struct servo *s, int32_t deg);
void servo_offset(struct servo *s, int32_t offset_deg);
void servo_set_offset_pos(struct servo *s, int32_t pos);

#endif

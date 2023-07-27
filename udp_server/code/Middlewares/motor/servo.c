#include "servo.h"
#include "stm32f7xx_ll_tim.h"

#define SERVO_PERIOD_PWM_US  20000
#define SERVO_RANGE_DEG      180
#define SERVO_HALF_RANGE_DEG (SERVO_RANGE_DEG / 2)

inline static void set_ccr(struct tim *tim, int32_t ccr_val)
{
    uint32_t ccr_val_max = tim->ccr_val_min + tim->ccr_vals_range;
    if (ccr_val < (int32_t)tim->ccr_val_min) {
        ccr_val = tim->ccr_val_min;
    } else if (ccr_val > (int32_t)ccr_val_max) {
        ccr_val = ccr_val_max;
    }
    *tim->ccr = ccr_val;
}

void servo_init(
    struct servo *s,
    uint32_t tim_arr_val,
    uint32_t pwm_width_min_us,
    uint32_t pwm_width_max_us)
{
    struct tim *tim = &s->tim;
    tim->ccr_val_min = tim_arr_val * pwm_width_min_us / SERVO_PERIOD_PWM_US;
    uint32_t ccr_val_max = tim_arr_val * pwm_width_max_us / SERVO_PERIOD_PWM_US;
    tim->ccr_vals_range = ccr_val_max - tim->ccr_val_min;
    *tim->ccr = (ccr_val_max + tim->ccr_val_min) / 2;
    s->offset_pos = 0;
}

void servo_moveto(struct servo *s, int32_t deg)
{
    struct tim *tim = &s->tim;
    deg += s->offset_pos;
    int32_t ccr_val = tim->ccr_vals_range * deg / SERVO_RANGE_DEG
                    + tim->ccr_vals_range / 2 + tim->ccr_val_min;
    set_ccr(tim, ccr_val);
}

void servo_offset(struct servo *s, int32_t offset_deg)
{
    struct tim *tim = &s->tim;
    int32_t ccr_offset_val = tim->ccr_vals_range * offset_deg / SERVO_RANGE_DEG;
    int32_t ccr_val = *tim->ccr + ccr_offset_val;
    set_ccr(tim, ccr_val);
}

void servo_set_offset_pos(struct servo *s, int32_t pos)
{
    s->offset_pos = pos;
}

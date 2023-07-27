#include "servo.h"
#include "stm32f7xx_ll_tim.h"

#define SERVO_PERIOD_PWM_US 20000

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

}

void servo_move(struct servo *s, uint32_t deg)
{
    struct tim *tim = &s->tim;
    uint32_t ccr_val = tim->ccr_vals_range * deg / 180 + tim->ccr_val_min;
    *tim->ccr = ccr_val;
}

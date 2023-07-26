#include "servo.h"
#include "stm32f7xx_ll_tim.h"

#define SERVO_PERIOD_PWM_US 20000

struct servo {
    volatile uint32_t *tim_ccr;
    volatile uint32_t *tim_arr;
    uint32_t tim_ccr_min;
    uint32_t tim_ccr_range;
};

void servo_init(struct servo *s, uint32_t pwm_width_min_us, uint32_t pwm_width_max_us)
{
    s->tim_ccr_min = *s->tim_arr * pwm_width_min_us / SERVO_PERIOD_PWM_US;
    uint32_t tim_ccr_max = *s->tim_arr * pwm_width_max_us / SERVO_PERIOD_PWM_US;
    s->tim_ccr_range = tim_ccr_max - s->tim_ccr_min;
}

void servo_move(struct servo *s, uint32_t deg)
{
    uint32_t ccr_val = s->tim_ccr_range * deg / 180 + s->tim_ccr_min;
    *s->tim_ccr = ccr_val;
}

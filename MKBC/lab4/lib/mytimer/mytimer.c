#include "mytimer.h"
#include <stm32h7xx.h>

static volatile uint32_t tick_counter = 0;

MyTimer mytimer_create(uint32_t period_ms) {
    MyTimer t = {systim_current_ms(), period_ms};
    return t;
}

void mytimer_restart(MyTimer *timer) { 
    timer->start_tick = systim_current_ms();
}

void mytimer_reset(MyTimer *timer, uint32_t period) {
    timer->period_ms = period;
    timer->start_tick = systim_current_ms();
}

bool mytimer_is_ready(MyTimer *timer) {
    return (systim_elapsed_ms(timer->start_tick) >= timer->period_ms);
}

uint32_t mytimer_elapsed_ms(MyTimer *timer) {
    return systim_elapsed_ms(timer->start_tick);
}
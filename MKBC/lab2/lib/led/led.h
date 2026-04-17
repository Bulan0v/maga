#pragma once

#include <stm32h7xx.h>

#define TOGGLE_BIT(REG, BIT) ((REG) ^= (BIT))

typedef enum {
    LED_GREEN  = 1,   // PB14
    LED_YELLOW = 2,   // PE1
    LED_RED    = 4,   // PB0
    LED_ALL    = 1|2|4
} led_t;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} led_pin_t;

static const led_pin_t led_pins[] = {
    [0] = {GPIOB, 14},
    [1] = {GPIOE, 1},
    [2] = {GPIOB, 0},
};

void led_enable(led_t led);
void led_toggle(led_t led);
void led_on(led_t led);
void led_off(led_t led);
void led_disable(led_t led);
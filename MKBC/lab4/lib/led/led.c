// led.c
#include "led.h"

static void enable_clock(GPIO_TypeDef *port) {
    if (port == GPIOB)
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    else if (port == GPIOE)
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
}

static void set_output(GPIO_TypeDef *port, uint16_t pin) {
    port->MODER = (port->MODER & ~(GPIO_MODER_MODER0 << (pin * 2)))
                | (GPIO_MODER_MODER0_0 << (pin * 2));
}

static void set_analog(GPIO_TypeDef *port, uint16_t pin) {
    port->MODER |= GPIO_MODER_MODER0 << (pin * 2);
}

static void pin_on(GPIO_TypeDef *port, uint16_t pin) {
    port->BSRR = 1 << pin;
}

static void pin_off(GPIO_TypeDef *port, uint16_t pin) {
    port->BSRR = 1 << (pin + 16);
}

static void pin_toggle(GPIO_TypeDef *port, uint16_t pin) {
    TOGGLE_BIT(port->ODR, 1 << pin);
}

static void for_each_pin(led_t led, void (*func)(GPIO_TypeDef*, uint16_t)) {
    uint32_t mask = led;
    while (mask) {
        int i = __builtin_ctz(mask);
        func(led_pins[i].port, led_pins[i].pin);
        mask &= mask - 1;
    }
}

void led_enable(led_t led) {
    uint32_t mask = led;
    uint32_t clocks = 0;
    while (mask) {
        int i = __builtin_ctz(mask);
        GPIO_TypeDef *port = led_pins[i].port;
        if (!(clocks & (1 << (uint32_t)port))) {
            enable_clock(port);
            clocks |= 1 << (uint32_t)port;
        }
        set_output(port, led_pins[i].pin);
        mask &= mask - 1;
    }
}

void led_toggle(led_t led) {
    for_each_pin(led, pin_toggle);
}

void led_on(led_t led) {
    for_each_pin(led, pin_on);
}

void led_off(led_t led) {
    for_each_pin(led, pin_off);
}

void led_disable(led_t led) {
    for_each_pin(led, set_analog);
}
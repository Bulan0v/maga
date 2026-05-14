#include "myled.h"

void myled_enable() {
    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);
    MODIFY_REG(GPIOE->MODER, GPIO_MODER_MODE1_Msk, GPIO_MODER_MODE1_0);
}

void myled_toggle() {
    TOGGLE_BIT(GPIOE->ODR, GPIO_ODR_OD1);
}

void myled_disable() {
    MODIFY_REG(GPIOE->MODER, GPIO_MODER_MODE1_Msk, GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1);
}
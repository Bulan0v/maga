#include "myled.h"
#include <stm32h7xx.h>

// void myled_enable() {
//     RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN; // включаем тактирование GPIOE
//     GPIOE->MODER &= ~GPIO_MODER_MODE1_Msk; // включаем режим "output" ...
//     GPIOE->MODER |= GPIO_MODER_MODE1_0; // ... для pin 1
// }

void myled_enable() {
    SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);
    MODIFY_REG(GPIOE->MODER, GPIO_MODER_MODE1_Msk, GPIO_MODER_MODE1_0);
}

// void myled_toggle() {
//     GPIOE->ODR ^= 2; // ; "исключающее или" c единицей меняет 0->1 и 1->0
// }

void myled_toggle() {
    TOGGLE_BIT(GPIOE->ODR, GPIO_ODR_OD1);
}

// void myled_disable() {
//     GPIOE->MODER &= ~GPIO_MODER_MODE1_Msk; // включаем режим "analog" ...
//     GPIOE->MODER |= GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1; // ... для pin 1
// }

void myled_disable() {
    MODIFY_REG(GPIOE->MODER, GPIO_MODER_MODE1_Msk, GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1);
}
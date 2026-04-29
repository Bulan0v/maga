#include "unity_config.h"
#include <stdio.h>
#include <stm32h7xx.h>
#include <vterm.h>

// Блокирующая задержка с помощью счетчика DWT (Data Watchpoint and Trace unit)
static void delay(int ms) {
    uint32_t cycles = SystemCoreClock / 1000 * ms;
    DWT->CTRL |= 1;
    DWT->CYCCNT = 0;
    while (DWT->CYCCNT < cycles)
    __asm("nop");
}

void unity_output_start() {
    vterm_init(115200);
    delay(1000); // Задеркжа для подготовки к приёму данных хост-компьютером
}

void unity_output_char(char ch) { putchar(ch); }
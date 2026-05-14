#include <stm32h7xx.h>
#include "systim.h"

static volatile uint32_t g_sys_counter_ms = 0;
static uint32_t g_ticks_in_mcs = 0;

void SysTick_Handler() {
    g_sys_counter_ms += 1;
}

uint32_t systim_current_ms() {
    return g_sys_counter_ms;
};

void systim_init(uint32_t mcu_clock) {
    g_ticks_in_mcs = mcu_clock / 1000000U;
    // PM0253, раздел 4.4, стр 212.
    // TODO Запус SysTick
    if (SysTick_Config(mcu_clock / 1000U)) { // 1 ms период
        while (1); // ошибка
    }
    NVIC_SetPriority(SysTick_IRQn, 15);
}

uint32_t systim_elapsed_ms(uint32_t from) {
    uint32_t now = systim_current_ms();
    if (now >= from)
        return now - from;
    else
        return (UINT32_MAX - from) + now + 1;
}

void systim_delay_ms(uint32_t ms) {
    uint32_t start = systim_current_ms();
    while (systim_elapsed_ms(start) < ms);
}

void systim_delay_mcs(uint32_t mcs) {
    if (mcs == 0) return;

    // Частота SysTick = SystemCoreClock (т.к. CLKSOURCE = 1)
    uint32_t ticks_per_us = SystemCoreClock / 1000000;   // количество тиков в 1 микросекунде
    uint32_t total_ticks = mcs * ticks_per_us;           // тиков для всей задержки

    // Максимальное значение 24-битного счётчика = 0xFFFFFF
    const uint32_t max_ticks = 0xFFFFFF;

    while (total_ticks > 0) {
        uint32_t block_ticks = total_ticks;
        if (block_ticks > max_ticks) block_ticks = max_ticks;

        // Запоминаем текущее значение счётчика (уменьшается)
        uint32_t start = SYST_CVR;
        uint32_t elapsed = 0;

        while (elapsed < block_ticks) {
            uint32_t current = SYST_CVR;
            if (current <= start) {
                elapsed += (start - current);
            } else {
                // Переполнение (счётчик дошёл до 0 и перезагрузился)
                elapsed += (start + (max_ticks + 1 - current));
            }
            start = current;
        }
        total_ticks -= block_ticks;
    }
}
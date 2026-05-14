/**
* Пример планировщика на базе программных таймеров
*/
#include "led.h"
#include "mytimer.h"
#include "stm32h7xx.h"
#include "vterm.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_iwdg.h"
#include <stdio.h>
// #include "crc32.h"

#define USER_BUTTON_PORT GPIOC
#define USER_BUTTON_PIN  LL_GPIO_PIN_13

typedef enum {
    MODE_SPEED_X1 = 0,   // период 2000 мс
    MODE_SPEED_X2 = 1,   // период 4000 мс
    MODE_OFF      = 2
} speed_mode_t;

static speed_mode_t current_mode = MODE_SPEED_X1;
static uint8_t current_led_index = 0;
static MyTimer running_light_timer;

// void SysTick_Handler() { mytimer_SysTickHandler(); }

/*******************************************************************************
* Программное меню *
*******************************************************************************/
#define NUM_COMMANDS 5
static void menu_show_tick() {
    uint32_t tick = mytimer_get_tick_counter();
    if (tick) {
        printf(u8"\nВремя работы %ld.%ld секунд", tick / 1000, tick % 1000);
    } else {
        printf(u8"\nСистеменый таймер не был настроен должным образом.");
    }
};

static void menu_show_title(void) {
    printf(u8"\r\n\n Меню приложения System clock is %ld "
           u8"MHz %s",
           SystemCoreClock / 1000000,
           u8"\n\r┌────────────┬───────────────┬────────────────┬────────────┬──────────┐"
           u8"\n\r│ 1:ShowTick │ 2: do_profile │  3: Test WDT   │      4:    │ 5: Reset │"
           u8"\n\r└────────────┴───────────────┴────────────────┴────────────┴──────────┘"
           u8"\n\r Выбор [1-6] > ");
};
typedef void (*handler_func_t)();
handler_func_t handlers[NUM_COMMANDS] = {menu_show_tick, 
                                        //  do_profile,
                                        //  test_watchdog,
                                         NULL,
                                         NULL, NULL,
                                         NVIC_SystemReset};

/*******************************************************************************
* Обработичик таймеров/задач *
*******************************************************************************/
led_t current_led = LED_GREEN;
void hear_rate_handler(MyTimer *timer) {
    mytimer_restart(timer);
    led_toggle(current_led);
}

void change_led_handler(MyTimer *timer) {
    mytimer_restart(timer);
    led_off(current_led);
    current_led = current_led == LED_GREEN ? LED_YELLOW : LED_GREEN;
}

void set_mode(speed_mode_t new_mode) {
    current_mode = new_mode;
    led_t leds[] = {LED_RED, LED_YELLOW, LED_GREEN};
    if (current_mode == MODE_OFF) {
        led_off(LED_ALL);
    } else {
        led_on(leds[current_led_index]);
        uint32_t period = (current_mode == MODE_SPEED_X1) ? 2000 : 4000;
        mytimer_reset(&running_light_timer, period);
    }
}

void running_light_handler(MyTimer *timer) {
    if (current_mode == MODE_OFF) return;
    led_t leds[] = {LED_RED, LED_YELLOW, LED_GREEN};
    led_off(leds[current_led_index]);
    current_led_index = (current_led_index + 1) % 3;
    led_on(leds[current_led_index]);
    mytimer_restart(timer);
}

// static void do_profile(void) {
//     // Определяем область .rodata
//     uint8_t *start = (uint8_t*)&_srodata;
//     uint32_t size = (uint32_t)&_erodata - (uint32_t)&_srodata;

//     if (size == 0) {
//         printf("\nОшибка: секция .rodata пуста\n");
//         return;
//     }

//     printf("\nПрофилирование CRC32 для .rodata: адрес 0x%08X, размер %lu байт\n",
//            (uint32_t)start, size);

    // RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;   // включить тактирование CRC
    // CRC->CR |= CRC_CR_RESET;
    // LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC);
//     // 1) Программный CRC
//     uint32_t t0 = systim_current_ms();
//     uint32_t crc_sw = calculate_CRC32_SW(start, size);
//     uint32_t t_sw = systim_elapsed_ms(t0);

//     // 2) Аппаратный CRC (8-бит)
//     t0 = systim_current_ms();
//     uint32_t crc_hw8 = calculate_CRC32_HW_8bit(start, size);
//     uint32_t t_hw8 = systim_elapsed_ms(t0);

//     // 3) Аппаратный CRC (32-бит)
//     t0 = systim_current_ms();
//     uint32_t crc_hw32 = calculate_CRC32_HW_32bit(start, size);
//     uint32_t t_hw32 = systim_elapsed_ms(t0);

//     printf("SW CRC32: 0x%08X, время %lu мс\n", crc_sw, t_sw);
//     printf("HW CRC32 (8bit): 0x%08X, время %lu мс\n", crc_hw8, t_hw8);
//     printf("HW CRC32 (32bit): 0x%08X, время %lu мс\n", crc_hw32, t_hw32);
// }

// static void iwdg_init(void) {
//     // Включить LSI (если ещё не включён)
//     LL_RCC_LSI_Enable();
//     while (!LL_RCC_LSI_IsReady());   // ждём готовности

//     // Разрешить доступ к регистрам IWDG (разблокировать)
//     LL_IWDG_Enable(IWDG);
//     // Ждать синхронизации (опционально)
//     while (LL_IWDG_IsReady(IWDG) == 0);

//     // Установить предделитель и счётчик
//     LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);   // PR=4 (2^4=16? В LL константа LL_IWDG_PRESCALER_64 соответствует делителю 64? Проверьте enum)
//     LL_IWDG_SetReloadCounter(IWDG, 2047);               // RL = 2047

//     // Перезагрузить счётчик и запустить
//     LL_IWDG_ReloadCounter(IWDG);
//     LL_IWDG_Enable(IWDG);
// }

// static void test_watchdog(void) {
//     printf("\nЗапуск бесконечного цикла. Watchdog перезагрузит МК через ~4 секунды.\n");
//     while(1) {
//         // Ничего не делаем – IWDG не перезагружается
//     }
// }

void menu_handler(MyTimer *timer) {
    mytimer_restart(timer);
    uint8_t ch = vterm_keypressed();
    if (ch > 0) {
        putchar(ch); // echo
        if (ch == '1') menu_show_tick();
        // else if (ch == '2') do_profile();
        // else if (ch == '3') test_watchdog();
        else if (ch == '5') NVIC_SystemReset();
        menu_show_title();
    }
    static uint32_t last_debounce = 0;
    static uint8_t last_state = 1;
    uint8_t cur_state = LL_GPIO_IsInputPinSet(USER_BUTTON_PORT, USER_BUTTON_PIN) ? 1 : 0;
    if (cur_state != last_state) {
        last_debounce = mytimer_get_tick_counter();
        last_state = cur_state;
    } else if (cur_state == 0 && (mytimer_get_tick_counter() - last_debounce) > 50) {
        set_mode((current_mode + 1) % 3);
        last_debounce = mytimer_get_tick_counter();
        while (LL_GPIO_IsInputPinSet(USER_BUTTON_PORT, USER_BUTTON_PIN) == 0);
        last_state = 1;
        last_debounce = mytimer_get_tick_counter();
    }
}

/*******************************************************************************
* Защитник пробуждения *
*******************************************************************************/
static void wait_for_blue_button_release(void) {
    led_enable(LED_ALL);
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC);
    LL_GPIO_InitTypeDef init = {0};
    LL_GPIO_StructInit(&init);
    init.Mode = LL_GPIO_MODE_INPUT;
    init.Pin = USER_BUTTON_PIN;
    LL_GPIO_Init(USER_BUTTON_PORT, &init);
    while (LL_GPIO_IsInputPinSet(USER_BUTTON_PORT, USER_BUTTON_PIN)) {
        led_on(LED_ALL);
    }
    led_off(LED_ALL);
}

/*******************************************************************************
* Суперцикл *
*******************************************************************************/
int main() {
    wait_for_blue_button_release();
    vterm_init(115200);
    mytimer_init(SystemCoreClock);
    led_enable(LED_ALL);
    // menu_show_title();

    set_mode(MODE_SPEED_X1);
    MyTimer menu_timer = mytimer_create(10);
    
    while (1) {
        if (mytimer_is_ready(&running_light_timer))
            running_light_handler(&running_light_timer);
        if (mytimer_is_ready(&menu_timer))
            menu_handler(&menu_timer);

        // LL_IWDG_ReloadCounter(IWDG);

        // Сон
        __WFE();
    }
}
#include <stm32h7xx.h>
#include <assert.h>
#include <stdio.h>
#include <vterm.h>

#define APP_SRAM_OFFSET 0x24000000
#define NUM_COMMANDS 6
#if NUM_COMMANDS > 9
#error NUM_COMMANDS must be less then 10 or change read_handler_index()
#endif

extern void HardFault_Handler();
uint32_t bootloader_SP = 0;
extern const char* get_last_error(void);
extern void clear_last_error(void);

typedef struct {
    uint32_t magic;
    char message[256];
} last_error_t;

static const char *gc_help_msg =
    u8"\n\r┌────────────┬──────────────┬────────────┬────────────┬────────────┬─────────────┐"
    u8"\n\r│ 1:BootSRAM │ 2:UsageFault │ 3:BusFault │ 4:MemFault │ 5:assert() │ 6:LastError │"
    u8"\n\r└────────────┴──────────────┴────────────┴────────────┴────────────┴─────────────┘"
    u8"\n\r Выбор [1-6] > ";

static void do_BootSRAM();
static void do_UsageFault();
static void do_MemFault();
static void do_BusFault();
static void do_Assert();
static void do_LastError();

typedef void (*handler_func_t)();

handler_func_t handlers[NUM_COMMANDS] = {do_BootSRAM, do_UsageFault, do_BusFault,
                                         do_MemFault, do_Assert,     do_LastError};

uint8_t read_handler_index() {
    while (vterm_keypressed() != 0)
    ;
    char str[2];
    int sz = vterm_gets(str, sizeof(str), 1);
    if (sz < 1)
        return UINT8_MAX;
    return str[0] >= '1' ? str[0] - '1' : UINT8_MAX;
}

void enable_fault_handlers() {
// Включить генерацию исключений для UsageFault; cм. PM0253, п. 4.3.7 на стр. 200
// SCB->CCR ....
// Разрешить генерацию исключений; см. PM0253, п. 4.3.9 на
// стр. 204 SCB->SHCSR ...

    SCB->CCR |= SCB_CCR_USERSETMPEND_Msk    // разрешить привилегированный доступ к PendSV
              | SCB_CCR_DIV_0_TRP_Msk;       // деление на ноль -> UsageFault

    // 2) Включить обработку этих исключений в регистре SHCSR
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
                | SCB_SHCSR_BUSFAULTENA_Msk
                | SCB_SHCSR_MEMFAULTENA_Msk;
}

int main() {
    vterm_init(115200);
    enable_fault_handlers();
    for (;;) {
        printf("\r\n System clock is %ld MHz %s", SystemCoreClock / 1000000, gc_help_msg);
        uint8_t handler_index = read_handler_index();
        if (handler_index < NUM_COMMANDS) {
            handlers[handler_index]();
        }
    }
    return 0;
}

/***************************** Обработчики команд ************************************/

void do_BootSRAM() {

    printf("\nJumping to SRAM app at %08lx....\n",APP_SRAM_OFFSET);
    // 1) Определить ТВП приложения, адреса начала стека и точки входа приложения
    const uint32_t* app_IV = (uint32_t*)(APP_SRAM_OFFSET);
    uint32_t app_end_stack = (*((uint32_t *)(app_IV)));
    void* app_entry = (void *)(*((uint32_t *)(APP_SRAM_OFFSET + 4)));

    // Доп.1.) Признак того, что был запуск приложения bootloader_SP != 0
    bootloader_SP = __get_MSP();

    // 2) Отключить все прерывания
    __disable_irq();

    // 3) заменить текущий адрес стека на начальный адрес стека приложения
    __set_MSP(app_end_stack);

    // 4) задать новый адрес таблицы векторов прерываний
    SCB->VTOR = app_IV;

    // Доп.2) Заменили обработчика HardFault в ТВП на собственный
    NVIC_SetVector(HardFault_IRQn, (uint32_t)HardFault_Handler);

    // 5) Безусловный переход на точку входу
    __ASM volatile("mov pc, %0" ::"r"(app_entry));
}

void do_UsageFault() {
    // Отслеживаемые ошибки задаются в SCB->UFSR (PM0253.rev5 стр. 209 )
    // Например, деление на ноль, Доступ к невыровненным данным

    printf("Generating UsageFault: division by zero...\n");
    volatile int a = 1;
    volatile int b = 0;
    volatile int c = a / b;    // деление на ноль вызовет UsageFault (если разрешено)
    (void)c;
}

void do_MemFault() {
    // Нарушнеие аттрибутов памяти
    // например, попытка выполнения кода из области памяти для переферийных устройств
    void *ptr = (void *)0x40000000;
    goto *ptr;
}
void do_BusFault() {
    // Ошибка доступа к памяти по шине
    // Например, попытка чтения из отсутствующей внешней памяти (0х60000000)
    printf("Generating BusFault: read from invalid address 0x6b000000...\n");
    volatile uint32_t *ptr = (uint32_t *)0x6b000000;
    volatile uint32_t dummy = *ptr;
    (void)dummy;
}

void do_Assert() { assert(!"Assertion example"); }

void do_LastError(void) {
    const char* err = get_last_error();
    if (err) {
        printf("\r\nLast error: %s\n", err);
    } else {
        printf("\r\nNo errors recorded.\n");
    }
}
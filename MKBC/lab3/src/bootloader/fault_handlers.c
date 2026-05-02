#include "stm32h7xx.h"
#include <stdio.h>
#include <stdarg.h>
#include <vterm.h>

extern uint32_t bootloader_SP;
extern uint32_t _lasterror_start;
extern uint32_t _lasterror_end;

typedef struct {
    uint32_t magic;     // 0xDEADBEEF – признак наличия ошибки
    char message[256];
} last_error_t;

static last_error_t g_last_error __attribute__((section(".lasterror")));

static void save_last_error(const char *fmt, ...) {
    if ((uint32_t)&_lasterror_end - (uint32_t)&_lasterror_start < sizeof(last_error_t))
        return;
    last_error_t *err = (last_error_t*)&_lasterror_start;
    err->magic = 0xDEADBEEF;
    va_list args;
    va_start(args, fmt);
    vsnprintf(err->message, sizeof(err->message), fmt, args);
    va_end(args);
}

void clear_last_error(void) {
    if ((uint32_t)&_lasterror_end - (uint32_t)&_lasterror_start >= sizeof(last_error_t)) {
        last_error_t *err = (last_error_t*)&_lasterror_start;
        err->magic = 0;
        err->message[0] = '\0';
    }
}

const char* get_last_error(void) {
    return (g_last_error.magic == 0xDEADBEEF) ? g_last_error.message : NULL;
}

void MemManage_Handler() {
    puts("\r\nMemory Management Fault exception!");
    uint32_t mmfsr = (SCB->CFSR) & SCB_CFSR_MEMFAULTSR_Msk;
    char buf[256];
    snprintf(buf, sizeof(buf), "MemManage Fault: MMFSR=0x%02lx", mmfsr);
    save_last_error("%s", buf);
    printf("MMFSR = 0x%02lx\n\r", mmfsr);
    if (mmfsr & 0x01) {
        puts("The processor attempted an instruction fetch from a location that "
             "does not permit execution");
    }
    if (mmfsr & 0x80)
        printf("MMFAR = 0x%lx\n\r", (SCB->MMFAR));
    NVIC_SystemReset();
}
void HardFault_Handler() {
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;

    char buf[256];
    snprintf(buf, sizeof(buf), "HardFault: HFSR=0x%08lx CFSR=0x%08lx", hfsr, cfsr);
    save_last_error("%s", buf);

    if (bootloader_SP) {
        __set_MSP(bootloader_SP);
        bootloader_SP = 0;
        vterm_init(115200);
        puts("\r\nApplication HardFault exception\r\n");
    } else {
        puts("\r\nBootloader HardFault exception\r\n");
    }

    // Проверяем, не вызван ли HardFault из-за необработанного BusFault/MemManage/UsageFault
    if (hfsr & SCB_HFSR_FORCED_Msk) {
        puts("  -> Forced HardFault (nested fault)");
        if (cfsr & SCB_CFSR_USGFAULTSR_Msk) {
            puts("     * Underlying UsageFault:");
            uint32_t ufsr = cfsr & SCB_CFSR_USGFAULTSR_Msk;
            if (ufsr & SCB_CFSR_DIVBYZERO_Msk) puts("       - Division by zero");
            if (ufsr & SCB_CFSR_UNALIGNED_Msk) puts("       - Unaligned access");
            if (ufsr & SCB_CFSR_NOCP_Msk) puts("       - No coprocessor");
            if (ufsr & SCB_CFSR_INVPC_Msk) puts("       - Invalid PC load");
            if (ufsr & SCB_CFSR_INVSTATE_Msk) puts("       - Invalid state");
            if (ufsr & SCB_CFSR_UNDEFINSTR_Msk) puts("       - Undefined instruction");
        }
        if (cfsr & SCB_CFSR_BUSFAULTSR_Msk) {
            puts("     * Underlying BusFault:");
            uint32_t bfsr = cfsr & SCB_CFSR_BUSFAULTSR_Msk;
            if (bfsr & SCB_CFSR_BFARVALID_Msk)
                printf("       - Address: 0x%08lx\n", SCB->BFAR);
            if (bfsr & SCB_CFSR_PRECISERR_Msk) puts("       - Precise data access error");
            if (bfsr & SCB_CFSR_IMPRECISERR_Msk) puts("       - Imprecise data access error");
            if (bfsr & SCB_CFSR_STKERR_Msk) puts("       - Stack access error");
        }
        if (cfsr & SCB_CFSR_MEMFAULTSR_Msk) {
            puts("     * Underlying MemManageFault:");
            uint32_t mmfsr = cfsr & SCB_CFSR_MEMFAULTSR_Msk;
            if (mmfsr & SCB_CFSR_MMARVALID_Msk)
                printf("       - Address: 0x%08lx\n", SCB->MMFAR);
            if (mmfsr & SCB_CFSR_MSTKERR_Msk) puts("       - Stack access error");
            if (mmfsr & SCB_CFSR_MUNSTKERR_Msk) puts("       - Unstacking error");
            if (mmfsr & SCB_CFSR_DACCVIOL_Msk) puts("       - Data access violation");
            if (mmfsr & SCB_CFSR_IACCVIOL_Msk) puts("       - Instruction access violation");
        }
    } else {
        puts("  -> HardFault without forced escalation");
    }

    NVIC_SystemReset();
}

// Bus Fault – выводит BFSR и адрес BFAR
void BusFault_Handler(void) {
    puts("\r\nBus Fault exception!");
    uint32_t bfsr = (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) >> 8;
    char buf[256];
    snprintf(buf, sizeof(buf), "Bus Fault: BFSR=0x%02lx BFAR=0x%08lx", bfsr, SCB->BFAR);
    save_last_error("%s", buf);
    printf("BFSR = 0x%02lx\n", bfsr);
    if (bfsr & (1 << 7)) {
        printf("  -> Fault address BFAR = 0x%08lx\n", SCB->BFAR);
    }
    NVIC_SystemReset();
}

// Usage Fault – выводит UFSR
void UsageFault_Handler(void) {
    puts("\r\nUsage Fault exception!");
    uint32_t ufsr = SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk;
    char buf[256];
    snprintf(buf, sizeof(buf), "Usage Fault: UFSR=0x%04lx", ufsr);
    save_last_error("%s", buf);
    printf("UFSR = 0x%04lx\n", ufsr);
    if (ufsr & (1 << 9)) {
        puts("  -> Division by zero");
    }
    if (ufsr & (1 << 8)) {
        puts("  -> Unaligned access");
    }
    NVIC_SystemReset();
}

// Переопределение __assert_func (вызывается assert())
void __assert_func(const char *file, int line, const char *func, const char *failedexpr) {
    // Сохраняем текущий указатель стека, если это приложение
    if (bootloader_SP == 0) {
        bootloader_SP = __get_MSP();
    } 
    char buf[256];
    snprintf(buf, sizeof(buf), "Assertion failed: %s, function %s, file %s, line %d",
             failedexpr, func ? func : "?", file, line);
    save_last_error("%s", buf);
    // Инициализируем терминал, если ещё не инициализирован (например, после HardFault)
    vterm_init(115200);
    printf("\r\nAssertion failed: %s, function %s, file %s, line %d\n",
           failedexpr, func ? func : "?", file, line);
    NVIC_SystemReset();
    while (1);
}
#include <stm32h7xx.h> // основной заголовочный файл CMSIS для МК серии H7
#include <myled.h>
// #include <led.h>
#include <stdio.h>
#include <vterm.h>

#define USE_PWR_DIRECT_SMPS_SUPPLY
#define SMPS

__attribute__((optimize("-O0"))) //директива отключает оптимизацию кода компилятором для этой функции
static void delay(int ms) {
    volatile int counter = SystemCoreClock / 1000 / 6 * ms ;
    while (counter > 0) counter -= 1;
}

int main() {
    static int counter = 0;
    vterm_init(115200);
    myled_enable(); // инициализация выввода светодиода
    while(1){
        myled_toggle(); // переключение «вкл <-> откл»
        delay(500); // пауза между переключениями
        printf("\r%s %d %s", "Светодиод был переключен", counter++, "раз(а)");
    };
}
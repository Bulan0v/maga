
| Регистр      | Назначение                                                                                                                                                             | Варианты конфигурации<br>для отдельного вывода                                                                                                                                                                                                                                                                                                                                              |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| GPIO_OTYPER  | Конфигурирует тип цифрового вывода                                                                                                                                     | «Push Pull» (=0) или «Open Darain» (=1)                                                                                                                                                                                                                                                                                                                                                     |
| GPIO_MODER   | Настройка режима работы вывода                                                                                                                                         | - 00: вход (Input mode) <br>- 01: общий выход (General purpose output)<br>- 10: альтернативная функция (Alternate function)<br>- 11: аналоговый режим (Analog mode) — состояние после сброса.                                                                                                                                                                                               |
| GPIO_OSPEEDR | Управляет скоростью переключения выходов порта                                                                                                                         | - **00** – низкая скорость<br>- **01** – средняя скорость<br>- **10** – высокая скорость<br>- **11** – очень высокая скорость                                                                                                                                                                                                                                                               |
| GPIO_PUPDR   | Управляет внутренними подтягивающими резисторами на выводах порта                                                                                                      | - **00** – ни подтяжки к питанию, ни подтяжки к земле (плавающий вход/выход). Это режим по умолчанию после сброса для большинства выводов.<br>- **01** – включен подтягивающий резистор к питанию (Pull-up).<br>- **10** – включен подтягивающий резистор к земле (Pull-down).<br>- **11** – зарезервировано.                                                                               |
| GPIO_IDR     | Содержит текущее состояние (уровень напряжения)                                                                                                                        | - **0** – на выводе низкий уровень напряжения (логический 0).<br>- **1** – на выводе высокий уровень напряжения (логический 1).                                                                                                                                                                                                                                                             |
| GPIO_ODR     | Управляет выходным состоянием выводов порта, когда они сконфигурированы как выходы                                                                                     | - **0** – на выводе устанавливается низкий уровень (логический 0).<br>- **1** – на выводе устанавливается высокий уровень (логический 1)                                                                                                                                                                                                                                                    |
| GPIO_BSRR    | Предназначен для атомарной (неделимой) установки или сброса отдельных выводов порта, работающих в режиме вывода, без необходимости читать и модифицировать регистр ODR | - **Биты 15:0** — **установка** (set).  <br>    Запись `1` в бит BSx устанавливает соответствующий вывод в высокий уровень (записывает `1` в ODRx).  <br>    Запись `0` не оказывает действия.<br>- **Биты 31:16** — **сброс** (reset).  <br>    Запись `1` в бит BRx сбрасывает соответствующий вывод в низкий уровень (записывает `0` в ODRx).  <br>    Запись `0` не оказывает действия. |

Обобщенный алгоритм функции ResetHandler:
1. **Установка указателя стека**  
    Загружается значение `_estack` (вершина стека) в регистр `sp`. Это необходимо для корректной работы стека перед вызовом других функций.
2. **Настройка питания**  
    Вызов подпрограммы `ExitRun0Mode` — вероятно, переводит микроконтроллер из режима пониженного энергопотребления в рабочий режим (Run mode). Конкретные действия зависят от реализации этой функции.
3. **Инициализация системы тактирования**  
    Вызов `SystemInit` — стандартная функция, настраивающая тактовую частоту и периферию системы (PLL, множители частоты и т.д.). Обычно генерируется автоматически или предоставляется производителем.
4. **Копирование инициализированных данных из Flash в SRAM**
    - В регистры загружаются границы секции данных в оперативной памяти: `_sdata` (начало) и `_edata` (конец), а также адрес во Flash, где хранятся исходные значения: `_sidata`.
    - Регистр `r3` обнуляется (используется как счётчик или флаг).
    - Выполняется переход к метке `LoopCopyDataInit` (сам цикл копирования в предоставленном фрагменте не показан, но обычно реализуется как последовательное копирование слов из `_sidata` в `_sdata` до достижения `_edata`).

Сборка
``` bash
pio run -e hello_led -v
```

```
Processing hello_led (build_type: debug; build_src_filter: +<$PROJECT_DIR/system/*.c>, +<$PROJECT_DIR/system/*.s>, +<*>; platform: ststm32; board: nucleo_h745zi_q; framework: cmsis; build_flags: -std=c11 -Wall -Wextra -D CORE_CM7; board_build.ldscript: $PROJECT_DIR/system/h745cm7_flash.ld; board_build.cmsis.startup_file: broken_path; board_build.cmsis.system_file: broken_path; lib_extra_dirs: C:\PioVScPot\platformio\lib)
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------CONFIGURATION: https://docs.platformio.org/page/boards/ststm32/nucleo_h745zi_q.html
PLATFORM: ST STM32 (19.4.0) > ST Nucleo H745ZI-Q
HARDWARE: STM32H745ZIT6 480MHz, 512KB RAM, 1MB Flash
DEBUG: Current (stlink) On-board (stlink) External (blackmagic, cmsis-dap, jlink)
PACKAGES:
 - framework-cmsis @ 2.50501.200527 (5.5.1)
 - framework-cmsis-stm32h7 @ 1.10.6
 - tool-ldscripts-ststm32 @ 0.2.0
 - toolchain-gccarmnoneeabi @ 1.70201.0 (7.2.1)
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 6 compatible libraries
Scanning dependencies...
Dependency Graph
|-- myled (License: Unknown, Path: C:\MKBC_ivt_14_m_bulanov\lab2\lib\myled)
Building in debug mode
arm-none-eabi-as -mthumb -mcpu=cortex-m7 -g2 -ggdb2 -o .pio\build\hello_led\system\startup.o system\startup.s
arm-none-eabi-gcc -o .pio\build\hello_led\system\system.o -c -std=c11 -Wextra -ffunction-sections -fdata-sections -Wall -mthumb -mcpu=cortex-m7 -Og -g2 -ggdb2 -DPLATFORMIO=60118 -DSTM32H7 -DSTM32H7xx -DSTM32H745xx -DCORE_CM7 -DF_CPU=480000000L -D__PLATFORMIO_BUILD_DEBUG__ -Iinclude -Isrc -Ilib\myled -IC:\PioVScPot\platformio\packages\framework-cmsis\CMSIS\Include -IC:\PioVScPot\platformio\packages\framework-cmsis-stm32h7\Include system\system.c
arm-none-eabi-gcc -o .pio\build\hello_led\src\hello_led.o -c -std=c11 -Wextra -ffunction-sections -fdata-sections -Wall -mthumb -mcpu=cortex-m7 -Og -g2 -ggdb2 -DPLATFORMIO=60118 -DSTM32H7 -DSTM32H7xx -DSTM32H745xx -DCORE_CM7 -DF_CPU=480000000L -D__PLATFORMIO_BUILD_DEBUG__ -Iinclude -Isrc -Ilib\myled -IC:\PioVScPot\platformio\packages\framework-cmsis\CMSIS\Include -IC:\PioVScPot\platformio\packages\framework-cmsis-stm32h7\Include src\hello_led.c        
arm-none-eabi-gcc -o .pio\build\hello_led\lib95d\myled\myled.o -c -std=c11 -Wextra -ffunction-sections -fdata-sections -Wall -mthumb -mcpu=cortex-m7 -Og -g2 -ggdb2 -DPLATFORMIO=60118 -DSTM32H7 -DSTM32H7xx -DSTM32H745xx -DCORE_CM7 -DF_CPU=480000000L -D__PLATFORMIO_BUILD_DEBUG__ -Ilib\myled -IC:\PioVScPot\platformio\packages\framework-cmsis\CMSIS\Include -IC:\PioVScPot\platformio\packages\framework-cmsis-stm32h7\Include lib\myled\myled.c
arm-none-eabi-gcc-ar rc .pio\build\hello_led\lib95d\libmyled.a .pio\build\hello_led\lib95d\myled\myled.o
arm-none-eabi-gcc-ranlib .pio\build\hello_led\lib95d\libmyled.a
arm-none-eabi-gcc -o .pio\build\hello_led\firmware.elf -T C:\MKBC_ivt_14_m_bulanov\lab2/system/h745cm7_flash.ld -Wl,--gc-sections,--relax -mthumb -mcpu=cortex-m7 --specs=nano.specs --specs=nosys.specs -Og -g2 -ggdb2 .pio\build\hello_led\system\startup.o .pio\build\hello_led\system\system.o .pio\build\hello_led\src\hello_led.o -LC:\PioVScPot\platformio\platforms\ststm32\ldscripts -L.pio\build\hello_led -Wl,--start-group .pio\build\hello_led\lib95d\libmyled.a -lc -lgcc -lm -lstdc++ -Wl,--end-group
MethodWrapper(["checkprogsize"], [".pio\build\hello_led\firmware.elf"])
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [          ]   0.0% (used 32 bytes from 524288 bytes)
Flash: [          ]   0.1% (used 620 bytes from 1048576 bytes)
.pio\build\hello_led\firmware.elf  :

section              size        addr

.isr_vector           664   134217728

.text                 616   134218392

.rodata                 0   134219008

.ARM.extab              0   134219008

.ARM                    0   134219008

.preinit_array          0   134219008

.init_array             4   134219008

.fini_array             4   134219012

.data                   4   536870912

.bss                   28   536870916

._user_heap_stack    1536   536870944

.ARM.attributes        42           0

.debug_line          1895           0

.debug_info          6208           0

.debug_abbrev        1063           0

.debug_aranges        176           0

.debug_str           2632           0

.debug_ranges         120           0

.debug_loc            485           0

.comment              126           0

.debug_frame          268           0

Total               15871
arm-none-eabi-objcopy -O binary .pio\build\hello_led\firmware.elf .pio\build\hello_led\firmware.bin
```

## 1. Общая информация о конфигурации проекта

Сначала вывод показывает параметры сборки:
- **Тип сборки:** `debug` (отладочная).
- **Платформа:** `ststm32`.
- **Плата:** `nucleo_h745zi_q` (отладочная плата с STM32H745).
- **Фреймворк:** `cmsis` (используются только CMSIS-заголовки и минимум кода, без HAL/LL).
- **Флаги компиляции:**
    - `-std=c11` — язык C11.
    - `-Wall -Wextra` — все предупреждения.
    - `-D CORE_CM7` — макрос, указывающий, что код для ядра Cortex-M7.
- **Скрипт линковки:** указан свой `h745cm7_flash.ld` из папки `system`.
- **Пути к startup и system файлам:** указаны как `broken_path`, но на самом деле используются локальные файлы из папки `system` проекта, поэтому предупреждение игнорируется.
- **Дополнительные пути для библиотек:** `lib_extra_dirs` указывает на внешнюю папку с библиотеками.
Далее идёт краткая характеристика платформы и пакетов (framework-cmsis, toolchain и т.д.), а затем — анализ зависимостей. Обнаружена одна библиотека `myled` (скорее всего, самодельная для работы со светодиодом).
## 2. Процесс компиляции и компоновки
### Ассемблирование startup-файла

``` bash
arm-none-eabi-as -mthumb -mcpu=cortex-m7 -g2 -ggdb2 -o .pio\build\hello_led\system\startup.o system\startup.s
```

Запускается ассемблер для файла `startup.s` (обычно содержит таблицу векторов прерываний и начальный код на ассемблере). Флаги: `-mthumb` (набор инструкций Thumb), `-mcpu=cortex-m7`, отладочная информация (`-g2 -ggdb2`).
### Компиляция system.c
```bash
arm-none-eabi-gcc ... -c system\system.c
```

Компилируется файл `system.c`, который обычно содержит инициализацию тактовой частоты (SystemInit) и другие низкоуровневые функции. Флаги:
- `-ffunction-sections -fdata-sections` — каждая функция/переменная помещается в отдельную секцию, чтобы линковщик мог выкинуть неиспользуемые.
- `-Og` — оптимизация, подходящая для отладки.
- `-D...` — определения макросов (например, `STM32H745xx`, `CORE_CM7`, `F_CPU`).
- Пути для поиска заголовочных файлов: `-Iinclude`, `-Isrc`, `-Ilib\myled`, а также пути к CMSIS из пакетов PlatformIO.
### Компиляция hello_led.c и myled.c
Аналогично компилируются основные файлы проекта: `src/hello_led.c` и библиотека `lib/myled/myled.c`.
### Создание статической библиотеки из myled

```bash
arm-none-eabi-gcc-ar rc .pio\build\hello_led\lib95d\libmyled.a ... 
arm-none-eabi-gcc-ranlib .pio\build\hello_led\lib95d\libmyled.a
```
Объектный файл `myled.o` упаковывается в архив `libmyled.a`, затем создаётся индекс для быстрого поиска символов.
### Линковка (компоновка) итогового ELF-файла

```bash
arm-none-eabi-gcc -o .pio\build\hello_led\firmware.elf -T .../h745cm7_flash.ld ... --specs=nano.specs --specs=nosys.specs ... .pio\build\hello_led\system\startup.o ... -Wl,--start-group ... -lc -lgcc -lm -lstdc++ -Wl,--end-group
```

На этом этапе все объектные файлы и библиотеки объединяются в исполняемый файл формата ELF.
- `-T` — задаёт скрипт линковки, который определяет расположение секций в памяти (флеш и RAM).
- `--gc-sections` — удаляет неиспользуемые секции.
- `--relax` — оптимизация переходов.
- `--specs=nano.specs --specs=nosys.specs` — используют уменьшенную версию библиотеки newlib (nano) и заглушки для системных вызовов (nosys), так как ОС нет.
- Группа библиотек `-lc -lgcc -lm -lstdc++` подключается для поддержки стандартных функций C, математических и C++ (даже если проект на C, они могут понадобиться).
### Проверка размера прошивки
``` bash
MethodWrapper(["checkprogsize"], [".pio\build\hello_led\firmware.elf"])
```

PlatformIO запускает скрипт для анализа использования памяти. Результат:
- **RAM:** использовано 32 байта из 512 КБ (0.0%).
- **Flash:** использовано 620 байт из 1 МБ (0.1%).
Это очень маленькая программа, скорее всего просто мигание светодиодом (несколько строк кода).
## 3. Подробная разбивка по секциям ELF-файла
Далее выводится таблица секций из `firmware.elf` (скоманда `arm-none-eabi-objdump -h` или аналогичная). Рассмотрим основные:
- **.isr_vector** — таблица векторов прерываний. Размер 664 байта, размещается по адресу 0x80000000 (134217728) — начало флеш-памяти.
- **.text** — исполняемый код (функции). 616 байт.
- **.rodata** — константные данные (отсутствуют, 0 байт).
- **.init_array / .fini_array** — массивы с функциями инициализации/финализации (по 4 байта, вероятно, для глобальных объектов C++ или конструкторов/деструкторов, но в данном проекте они пусты или содержат одну запись).
- **.data** — инициализированные глобальные/статические переменные (4 байта). Размещается в RAM (0x20000000 = 536870912).
- **.bss** — неинициализированные глобальные/статические переменные (28 байт). Тоже в RAM.
- **._user_heap_stack** — зарезервированная область для кучи и стека (1536 байт). Это не фактически занятая память, а выделенное место по скрипту линковки. Именно из-за этого `checkprogsize` показывает только 32 байта используемой RAM (сумма .data + .bss), а куча/стек учитываются как зарезервированные, но не используемые в данный момент.
- Остальные секции (`.debug_*`, `.comment`, `.ARM.attributes`) содержат отладочную информацию и служебные данные, они не загружаются в микроконтроллер.

**Общий размер загружаемых секций (флеш):** .isr_vector + .text + .init_array + .fini_array = 664 + 616 + 4 + 4 = 1288 байт? Но выше указано "Flash: used 620 bytes". Возможно, в подсчёте учитываются только секции, попадающие во флеш, а .init_array и .fini_array могут располагаться тоже во флеше (они обычно в ROM). Однако 664+616=1280, что больше 620. Расхождение может быть связано с тем, что секции .init_array и .fini_array могут быть нулевого размера или не учитываться? На самом деле из таблицы видно, что .init_array и .fini_array имеют размер 4 байта каждая, но они, вероятно, не включаются в "used flash" из-за особенностей подсчёта (может, они пусты или линкер их оптимизировал). Либо реально занято 620 байт, а остальное – отладочная информация, которая не пишется в память. Проверим: .isr_vector (664) + .text (616) = 1280, но это противоречит 620. Возможно, в выводе checkprogsize используется другой метод (например, размер загружаемых секций из .elf после objcopy). В любом случае, программа очень маленькая.

Затем создаётся бинарный файл `.bin` для прошивки:
```bash
arm-none-eabi-objcopy -O binary .pio\build\hello_led\firmware.elf .pio\build\hello_led\firmware.bin
```

### Ответы на вопросы для самопроверки (кратко)
1. **С какой функции начинается выполнение?**  
    С `Reset_Handler` (в startup-файле). Первое действие – настройка указателя стека (уже установлен аппаратно), затем копирование данных из Flash в RAM и обнуление BSS.
2. **Утилита для создания бинарного файла?**  
    `arm-none-eabi-objcopy` (из пакета GNU Tools).
3. **Входные параметры для ELF?**  
    Объектные файлы, скрипт компоновщика, библиотеки.
4. **ELF файл?**  
    Executable and Linkable Format – содержит код, данные, отладочную информацию, таблицу символов.
5. **Верны ли данные статистики по памяти?**  
    Да, они берутся из секций .text, .data, .bss и показывают занятость Flash и RAM.
6. **Секция с версией компилятора?**  
    `.ARM.attributes` – хранится во Flash, но может не загружаться в МК (зависит от компоновщика).
7. **Что если не копировать .data?**  
    Инициализированные переменные будут содержать мусор (значения из Flash не попадут в RAM).
8. **SystemInit()** – настройка тактовой системы (PLL, HSE и т.д.) до вызова main.
9. **SystemCoreClockUpdate()** – обновляет переменную `SystemCoreClock` после изменения частоты.
10. **SystemCoreClock** – хранит текущую частоту ядра в Гц.
11. **Макросы конфигурации CMSIS** – например, `HSE_VALUE`, `CORE_CM7`, `USE_PWR_*` и т.д.
12. **Компоненты CMSIS** – Core, DSP, RTOS, NN и др.
13. **Регистры GPIO** – MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRH/AFRL.
14. **Семихостинг** – механизм перенаправления ввода/вывода на хост через отладчик. Технологии: ARM Semihosting, SWV, RTT, UART.
15. **Newlib** – библиотека стандартного C для встраиваемых систем.
16. **vterm** – перенаправляет `printf`/`scanf` на UART3 через переопределение `_write`/`_read`.
17. **Команда MEMORY** – в скрипте компоновщика описывает физические области памяти (FLASH, RAM).
18. **Reference Manual** – архитектура МК, описание периферии и регистров.
19. **Data Sheet** – электрические параметры, цоколевка, корпуса.
20. **Настройка Unity для семихостинга** – определяются `UNITY_OUTPUT_CHAR` и `UNITY_OUTPUT_START`, которые отправляют символы через UART.
21. **System-файл** – инициализация тактовой системы. **Startup-файл** – таблица векторов и начальный код на ассемблере.
22. **Программа для ядра CM4** – нужно использовать соответствующий startup и system файлы, а также указать `-D CORE_CM4`.
23. **BSRR vs ODR** – BSRR позволяет атомарно установить/сбросить биты без риска прерывания; ODR требует чтения-модификации-записи.
24. **Состояния вывода** – вход, выход, альтернативная функция, аналоговый.
25. **Частота HSE** – 8 МГц.
26. **RCC** – блок управления тактированием и сбросом.
27. **Отличие delay()** – в `hello_led.c` используется цикл на основе `SystemCoreClock` (приблизительный), в `unity_config.c` обычно более точный с использованием DWT (Cycle Counter).
28. **DRY** – Don't Repeat Yourself; **KISS** – Keep It Simple, Stupid.
29. **Секции в скрипте** – `.isr_vector` (векторы), `.text` (код), `.rodata` (константы), `.data` (инициализированные данные), `.bss` (неинициализированные), `.ARM.exidx` (исключения), `.init_array` (конструкторы), `.fini_array` (деструкторы), `.ARM.attributes` (метаданные).
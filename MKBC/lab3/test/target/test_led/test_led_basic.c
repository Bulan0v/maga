#include <stm32h7xx.h>
#include <led.h>
#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

// Вспомогательная функция для чтения состояния пина (возвращает 0 или ненулевое значение)
static uint32_t read_pin(GPIO_TypeDef *port, uint32_t pin) {
    return port->ODR & (1 << pin);
}

// ----- Тесты для функции переключения (toggle) -----

void test_led_green_toggle(void) {
    led_enable(LED_GREEN);
    uint32_t initial = read_pin(GPIOB, 14);
    led_toggle(LED_GREEN);
    TEST_ASSERT_NOT_EQUAL(initial, read_pin(GPIOB, 14));
    led_toggle(LED_GREEN);
    TEST_ASSERT_EQUAL(initial, read_pin(GPIOB, 14));
    led_disable(LED_GREEN);
}

void test_led_red_toggle(void) {
    led_enable(LED_RED);
    uint32_t initial = read_pin(GPIOB, 0);
    led_toggle(LED_RED);
    TEST_ASSERT_NOT_EQUAL(initial, read_pin(GPIOB, 0));
    led_toggle(LED_RED);
    TEST_ASSERT_EQUAL(initial, read_pin(GPIOB, 0));
    led_disable(LED_RED);
}

void test_led_yellow_toggle(void) {
    led_enable(LED_YELLOW);
    uint32_t initial = read_pin(GPIOE, 1);
    led_toggle(LED_YELLOW);
    TEST_ASSERT_NOT_EQUAL(initial, read_pin(GPIOE, 1));
    led_toggle(LED_YELLOW);
    TEST_ASSERT_EQUAL(initial, read_pin(GPIOE, 1));
    led_disable(LED_YELLOW);
}

void test_led_all_toggle(void) {
    led_enable(LED_ALL);
    uint32_t init_green = read_pin(GPIOB, 14);
    uint32_t init_red   = read_pin(GPIOB, 0);
    uint32_t init_yellow = read_pin(GPIOE, 1);

    led_toggle(LED_ALL);

    TEST_ASSERT_NOT_EQUAL(init_green, read_pin(GPIOB, 14));
    TEST_ASSERT_NOT_EQUAL(init_red,   read_pin(GPIOB, 0));
    TEST_ASSERT_NOT_EQUAL(init_yellow, read_pin(GPIOE, 1));

    led_toggle(LED_ALL);

    TEST_ASSERT_EQUAL(init_green, read_pin(GPIOB, 14));
    TEST_ASSERT_EQUAL(init_red,   read_pin(GPIOB, 0));
    TEST_ASSERT_EQUAL(init_yellow, read_pin(GPIOE, 1));

    led_disable(LED_ALL);
}

// ----- Тесты для функций включения/выключения (on/off) -----

void test_led_green_on_off(void) {
    led_enable(LED_GREEN);
    led_on(LED_GREEN);
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOB, 14));
    led_off(LED_GREEN);
    TEST_ASSERT_EQUAL(0, read_pin(GPIOB, 14));
    led_disable(LED_GREEN);
}

void test_led_red_on_off(void) {
    led_enable(LED_RED);
    led_on(LED_RED);
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOB, 0));
    led_off(LED_RED);
    TEST_ASSERT_EQUAL(0, read_pin(GPIOB, 0));
    led_disable(LED_RED);
}

void test_led_yellow_on_off(void) {
    led_enable(LED_YELLOW);
    led_on(LED_YELLOW);
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOE, 1));
    led_off(LED_YELLOW);
    TEST_ASSERT_EQUAL(0, read_pin(GPIOE, 1));
    led_disable(LED_YELLOW);
}

void test_led_all_on_off(void) {
    led_enable(LED_ALL);

    led_on(LED_ALL);
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOB, 14));
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOB, 0));
    TEST_ASSERT_NOT_EQUAL(0, read_pin(GPIOE, 1));

    led_off(LED_ALL);
    TEST_ASSERT_EQUAL(0, read_pin(GPIOB, 14));
    TEST_ASSERT_EQUAL(0, read_pin(GPIOB, 0));
    TEST_ASSERT_EQUAL(0, read_pin(GPIOE, 1));

    led_disable(LED_ALL);
}

// ----- Запуск всех тестов -----

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_led_green_toggle);
    RUN_TEST(test_led_red_toggle);
    RUN_TEST(test_led_yellow_toggle);
    RUN_TEST(test_led_all_toggle);
    RUN_TEST(test_led_green_on_off);
    RUN_TEST(test_led_red_on_off);
    RUN_TEST(test_led_yellow_on_off);
    RUN_TEST(test_led_all_on_off);
    return UNITY_END();
}
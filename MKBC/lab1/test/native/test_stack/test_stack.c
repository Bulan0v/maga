#include <unity.h>
#include "dynlist.h"   // содержит всё необходимое, включая istack

static istack_handle stack;

void setUp(void) {
    stack = istack_new();
    TEST_ASSERT_NOT_NULL(stack);
}

void tearDown(void) {
    istack_free(stack);
}

// Тест: новый стек пуст (pop должен вернуть 0)
void test_stack_new_is_empty(void) {
    TEST_ASSERT_EQUAL_INT(0, istack_pop(stack));
}

// Тест: один push / один pop
void test_stack_push_pop_one(void) {
    istack_push(stack, 42);
    TEST_ASSERT_EQUAL_INT(42, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(0, istack_pop(stack));   // теперь пусто
}

// Тест: порядок LIFO (последний зашёл – первый вышел)
void test_stack_lifo_order(void) {
    int values[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        istack_push(stack, values[i]);
    }
    for (int i = 4; i >= 0; i--) {
        TEST_ASSERT_EQUAL_INT(values[i], istack_pop(stack));
    }
    TEST_ASSERT_EQUAL_INT(0, istack_pop(stack));
}

// Тест: многократные push/pop
void test_stack_multiple_ops(void) {
    istack_push(stack, 10);
    istack_push(stack, 20);
    TEST_ASSERT_EQUAL_INT(20, istack_pop(stack));
    istack_push(stack, 30);
    TEST_ASSERT_EQUAL_INT(30, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(10, istack_pop(stack));
    TEST_ASSERT_EQUAL_INT(0, istack_pop(stack));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stack_new_is_empty);
    RUN_TEST(test_stack_push_pop_one);
    RUN_TEST(test_stack_lifo_order);
    RUN_TEST(test_stack_multiple_ops);
    return UNITY_END();
}
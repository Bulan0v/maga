#include <unity.h>
#include <dynlist.h>
#include <string.h>

/*======= Фиксированные данные для тестирования (фиктуры) =========================*/
#define FIXTURE_SIZE 3
const int fixture_data_init[FIXTURE_SIZE] = {1, 2, 3};
int fixture_data[FIXTURE_SIZE];
Dynlist list;
void clean_func(void *val) { *(int *)val = 0; }
static void incrementor(void *val) { *(int *)val += 1; }
/*===================================================================================*/

void setUp(void) {
    memcpy(fixture_data, fixture_data_init, sizeof(fixture_data));
    list = dynlist_new(clean_func);
}

void tearDown(void) { dynlist_free(list); }

void test_dynlist_new_created_empty(void) { TEST_ASSERT_NULL(dynlist_begin(list)); }

void test_dynlist_free_called_clean_callback(void) {
    for (int i = 0; i < FIXTURE_SIZE; i++) {
        dynlist_append(list, &fixture_data[i]);
    }
    dynlist_free(list);
    list = NULL; // предотвратить повторное удлание в tearDown()
    TEST_ASSERT_EACH_EQUAL_INT(0, fixture_data, FIXTURE_SIZE);
}

void test_dynlist_append_data_in_place(void) {
    // Помещаем данные в спиок
    for (int i = 0; i < FIXTURE_SIZE; i++) {
        dynlist_append(list, &fixture_data[i]);
    }
    // Выбираем данные из списка в массив actual_data
    int actual_data[FIXTURE_SIZE] = {0};
    DynlistNode *node = dynlist_begin(list);
    int index = 0;
    while (node != NULL) {
        TEST_ASSERT_TRUE_MESSAGE(index < FIXTURE_SIZE, "List has more elements, than expected");
        actual_data[index++] = *(int *)dynlist_value(node);
        node = dynlist_next(node);
    }
    // Сравниваем
    TEST_ASSERT_EQUAL_INT_ARRAY(fixture_data, actual_data, FIXTURE_SIZE);
}

void test_dynlist_foreach_called(void) {
    // Помещаем данные в спиок
    for (int i = 0; i < FIXTURE_SIZE; i++) {
        dynlist_append(list, &fixture_data[i]);
    }
    // Вызов
    dynlist_foreach(list, incrementor);
    // Проверка
    DynlistNode *node = dynlist_begin(list);
    for (int i = 0; i < FIXTURE_SIZE; i++) {
        TEST_ASSERT_NOT_NULL_MESSAGE(node, "List is shorter, than expected");
        incrementor(&fixture_data[i]);
        TEST_ASSERT_EQUAL_INT(fixture_data[i], *(int *)dynlist_value(node));
        node = dynlist_next(node);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dynlist_new_created_empty);
    RUN_TEST(test_dynlist_free_called_clean_callback);
    RUN_TEST(test_dynlist_append_data_in_place);
    RUN_TEST(test_dynlist_foreach_called);
    return UNITY_END();
}
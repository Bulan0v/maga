#include <dynlist.h>
#include <stdio.h>

static void handle_item_puts(void *val) {
    puts((const char *)val);
}

int main() {
    Dynlist list = dynlist_new(NULL);
    dynlist_append(list, u8"Привет МИЭТ!");
    dynlist_prepend(list,u8"Сказал студент: ");
    dynlist_foreach(list, handle_item_puts);
    dynlist_free(list);
}
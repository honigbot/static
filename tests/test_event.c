#include <stdio.h>
#include "static/event.h"

void function1(const char * message, int i) {
    printf("[0] %s %d\n", message, i);
}

void function2(const char * message, int i) {
    printf("[1] %s %d\n", message, i);
}

void function3(const char * message, int i) {
    printf("[2] %s %d\n", message, i);
}

int main() {
    static_event_t event = {.first=0};
    static_callback_t callbacks[3];

    static_event_attach(&callbacks[0], &event, function1);
    static_event_attach(&callbacks[1], &event, function2);
    static_event_attach(&callbacks[2], &event, function3);
    static_event_trigger(&event, "hello!", 1);

    static_event_detach(&callbacks[1]);
    static_event_trigger(&event, "hello!", 1);

    static_event_detach(&callbacks[2]);
    static_event_trigger(&event, "hello!", 1);

    static_event_detach(&callbacks[0]);
    static_event_trigger(&event, "hello!", 1);

    return 0;
}

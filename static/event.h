typedef struct static_callback_t static_callback_t;
typedef struct static_event_t static_event_t;

typedef struct static_callback_t {
    void (*function)();
    static_callback_t * next;
    static_callback_t * previous;
    static_event_t * event;
} static_callback_t;

typedef struct static_event_t {
    static_callback_t * first;
} static_event_t;

void static_event_attach(static_callback_t * next_head, static_event_t * event, void (*function)()) {
    static_callback_t * last_head = event->first;

    if(last_head == 0) {
        // 'event' has no callbacks, 'next_head' is the first
        next_head->previous = 0;
        next_head->next = 0;
        next_head->event = event;
        event->first = next_head;
    } 
    else {
        // 'event' has callbacks, enqueue 'next_head' as new head
        last_head->previous = next_head;
        next_head->next = last_head;
        next_head->previous = 0;
        next_head->event = event;
        event->first = next_head;
    }

    next_head->function = function;
    event->first = next_head;
}

void static_event_detach(static_callback_t * callback) {
    static_callback_t * previous = callback->previous;
    static_callback_t * next = callback->next;
    if(previous != 0 && next != 0) {
        // 'callback' was a in between, link 'next' and 'previous'
        previous->next = next;
        next->previous = previous;
        return;
    }

    if(previous == 0 && next != 0) {
        // 'callback' was a head, make 'next' the new head
        next->previous = 0;
        callback->event->first = next;
        return;
    }

    if(previous != 0 && next == 0) {
        // 'callback' was a tail, make 'previous' the new tail
        previous->next = 0;
        return;
    }
    
    if(previous == 0 && next == 0) {
        // 'callback' the only callback
        callback->event->first = 0;
        return;
    }
}

#define static_event_trigger(event, ...) {\
    static_callback_t * current = (event)->first;\
    while(current != 0) {\
        current->function(__VA_ARGS__);\
        current = current->next;\
    }\
}

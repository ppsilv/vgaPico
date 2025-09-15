#ifndef __CURSOR_H__
#define __CURSOR_H__
#include "pico/stdlib.h"

typedef struct {
    short x, y;
    bool visible;
    bool enabled;
    uint32_t blink_interval;
    repeating_timer_t timer;
    unsigned char fontsize;
} cursor_t;   

bool cursor_blink_callback(repeating_timer_t *rt);
void cursor_init();
void cursor_set_position(short x, short y);
void cursor_enable(bool enable);

#endif
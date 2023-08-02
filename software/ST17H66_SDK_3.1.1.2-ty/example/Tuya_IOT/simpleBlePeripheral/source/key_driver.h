#ifndef _KEY_DRIVER_H 
#define _KEY_DRIVER_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum{

    KEY_1 = 1,

    KEY_2,

    KEY_3,

    KEY_1_2,

    KEY_2_3,

    KEY_MAX
};

enum{

    KEY_EVENT_DOWN = 1,

    KEY_EVENT_UP,

    KEY_EVENT_LONG,

    KEY_EVENT_MAX
};

#define KEY_TIME_PRO_EVENT 0x0001

typedef void (*key_event_cb)(uint8_t key_value, uint8_t event);

void key_task_init(uint8_t task_id);

uint16_t key_task_event(uint8_t task_id, uint16_t events);

void key_set_event_cb(key_event_cb cb);

#ifdef __cplusplus
}
#endif
#endif	// _KEY_DRIVER_H


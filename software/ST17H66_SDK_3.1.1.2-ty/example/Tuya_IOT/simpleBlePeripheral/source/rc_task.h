#ifndef _RC_TASK_H 
#define _RC_TASK_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "rc_switch.h"

typedef struct
{
    uint32_t send_value;

    uint16_t send_pulse_len;

    uint8_t send_protocol;

    uint8_t send_bit_len;
    
}rc_send_data_t;

enum{

    RC_RECEIVE_AVAILABLE = 0,
};

#define RC_TIME_PRO_EVENT 0x0001
#define RC_SEND_START_EVENT 0x0002
#define RC_SEND_STOP_EVENT 0x0004

typedef void (*rc_event_cb)(uint8_t event, void *args);

void rc_task_init(uint8_t task_id);

uint16_t rc_task_event(uint8_t task_id, uint16_t events);

uint8_t rc_send(rc_send_data_t *data);

uint8_t rc_rec_start(void);

uint8_t rc_rec_stop(void);

void rc_set_event_cb(rc_event_cb cb);

#ifdef __cplusplus
}
#endif
#endif	// _RC_TASK_H


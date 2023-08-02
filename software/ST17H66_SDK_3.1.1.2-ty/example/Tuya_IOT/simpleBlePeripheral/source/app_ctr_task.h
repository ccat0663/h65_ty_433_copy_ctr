#ifndef _APP_CTR_TASK_H 
#define _APP_CTR_TASK_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define APP_TIME_PRO_EVENT 0x0001
#define APP_SEND_EVENT 0x0002
#define APP_SEND_ONCE_EVENT 0x0004

void app_ctr_task_init(uint8_t task_id);

uint16_t app_ctr_task_event(uint8_t task_id, uint16_t events);

void app_ctr_rc_send_once(uint8_t index);

#ifdef __cplusplus
}
#endif
#endif	// _APP_CTR_TASK_H


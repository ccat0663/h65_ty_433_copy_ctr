
#ifndef _TY_APP_H 
#define _TY_APP_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

#define TY_UART_REC_DATA 0x0001
#define TY_KEY_LONG_EVENT   0x0002
#define TY_SEND_433_EVENT   0x0004

void ty_mcu_sdk_app_init(uint8_t task_id);

uint16_t ty_mcu_sdk_app_event(uint8_t task_id, uint16_t events);

void ty_uart_send_byte(uint8_t data);

void ty_rec_open_cmd(void);
void ty_rec_close_cmd(void);
void ty_rec_pause_cmd(void);

#ifdef __cplusplus
}
#endif
#endif	// _TY_APP_H


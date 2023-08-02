#ifndef _LED_DRIVER_H 
#define _LED_DRIVER_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum{

    LED_INIT = 0,

    LED_KEY_RC_DATA_SEND,           //按键发送数据
    LED_KEY_RC_NO_DATA_SEND,        //按键按下没数据发送
    LED_KEY_RC_REC_DATA,            //正在对码
    LED_KEY_RC_REC_DATA_DONE,       //对码成功
    LED_KEY_RC_DEL_DATA,            //清码成功

    LED_BLE_CONNECT,

    LED_NET_CFG,
    LED_NET_CONNECT,
    LED_NET_CON_SUC,
    LED_CLOUD_CON_SUC,
    LED_WIFI_LOW_POWER,

    LED_MAX

}led_status_t;

typedef enum{

    LED_BLUE = 0,
    LED_RED,

    LED_NUM_MAX
}led_num_t;

#define LED_TIME_PRO_EVENT 0x0001

void led_task_init(uint8_t task_id);

uint16_t led_task_event(uint8_t task_id, uint16_t events);

uint8_t led_pro(uint16_t tick_ms);

void led_set_status(led_num_t num, led_status_t status);
void led_clr_status(led_num_t num);
void led_reset_status(led_num_t num, led_status_t status);
led_status_t led_get_status(led_num_t num);

#ifdef __cplusplus
}
#endif
#endif	// _LED_DRIVER_H


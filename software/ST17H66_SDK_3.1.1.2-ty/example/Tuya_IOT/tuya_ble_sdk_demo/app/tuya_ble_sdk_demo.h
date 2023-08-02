/**
****************************************************************************
* @file      tuya_ble_sdk_demo.h
* @brief     tuya_ble_sdk_demo
* @author    suding
* @version   V1.0.0
* @date      2020-04
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TUYA_BLE_SDK_DEMO_H__
#define __TUYA_BLE_SDK_DEMO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "stdint.h"
#include "ty_dev_config.h"

/*********************************************************************
 * CONSTANT
 */
//PID - product id, DID - device id
//FIR - firmware, FVER - firmware version, HVER - hardware version

#define TY_DEVICE_NAME        "tycy"
#define TY_DEVICE_PID         TY_DEVICE_PID_CONFIG     
#define TY_DEVICE_MAC         TY_DEVICE_MAC_CONFIG     
#define TY_DEVICE_DID         TY_DEVICE_DID_CONFIG     
#define TY_DEVICE_AUTH_KEY    TY_DEVICE_AUTH_KEY_CONFIG

#define TY_DEVICE_FIR_NAME    "tuya_ble_sdk_demo"
#define TY_DEVICE_FVER_NUM    0x00000201
#define TY_DEVICE_FVER_STR    "2.1"
#define TY_DEVICE_HVER_NUM    0x00000100
#define TY_DEVICE_HVER_STR    "1.0"

#define TY_ADV_INTERVAL       1000///100   //range: 20~10240ms
#define TY_CONN_INTERVAL_MIN  180   //range: 7.5~4000ms
#define TY_CONN_INTERVAL_MAX  200   //range: 7.5~4000ms

//event id
typedef enum {
    APP_EVT_0,
    APP_EVT_1,
    APP_EVT_2,
    APP_EVT_3,
    APP_EVT_4,
} custom_evtid_t;

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
typedef struct {
    uint8_t dp_id;
    uint8_t dp_type;
    uint8_t dp_data_len;
    uint8_t dp_data[256];
} demo_dp_t;

typedef struct {
    uint32_t len;
    uint8_t  value[];
} custom_evt_data_t;
#pragma pack()

typedef void (*tuya_ble_app_master_result_handler_t)(uint32_t evt, uint8_t* buf, uint32_t size);

/*********************************************************************
 * EXTERNAL VARIABLE
 */
extern demo_dp_t g_cmd;
extern demo_dp_t g_rsp;

/*********************************************************************
 * EXTERNAL FUNCTION
 */
void tuya_ble_sdk_demo_init(void);
void tuya_ble_custom_evt_send(custom_evtid_t evtid);
void tuya_ble_custom_evt_send_with_data(custom_evtid_t evtid, void* buf, uint32_t size);
void tuya_ble_disconnect_and_reset_timer_init(void);
void tuya_ble_update_conn_param_timer_init(void);
void tuya_ble_disconnect_and_reset_timer_start(void);
void tuya_ble_update_conn_param_timer_start(void);
void tuya_ble_chx_key_pro_timer_init(void);
void tuya_ble_chx_key_pro_timer_start(void);


#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_SDK_DEMO_H__


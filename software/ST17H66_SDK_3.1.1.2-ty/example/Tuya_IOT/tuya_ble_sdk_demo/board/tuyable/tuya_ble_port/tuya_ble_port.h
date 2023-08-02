/**
****************************************************************************
* @file      
* @brief     
* @author    suding
* @version   V1.0.0
* @date      2020-04
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TUYA_BLE_PORT_H__
#define __TUYA_BLE_PORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
//#include "app_util_platform.h"
#include "tuya_ble_config.h"
#include "ty_system.h"
#include "elog.h"
#include "log.h"

/*********************************************************************
 * CONSTANT
 */
#if (TUYA_BLE_LOG_ENABLE||TUYA_APP_LOG_ENABLE)
    #define TUYA_BLE_PRINTF(...)        log_d(__VA_ARGS__)
    #define TUYA_BLE_HEXDUMP(...)       ty_system_log_hexdump("", 8, __VA_ARGS__)
#else
    #define TUYA_BLE_PRINTF(...)
    #define TUYA_BLE_HEXDUMP(...)
#endif

#define tuya_ble_device_enter_critical()        \
    HAL_ENTER_CRITICAL_SECTION();

#define tuya_ble_device_exit_critical()         \
       HAL_EXIT_CRITICAL_SECTION(); 

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */


#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_PORT_H__

tuya_ble_status_t tuya_ble_gap_advertising_adv_data_update(uint8_t const* p_ad_data, uint8_t ad_len);
tuya_ble_status_t tuya_ble_gap_advertising_scan_rsp_data_update(uint8_t const* p_sr_data, uint8_t sr_len);
tuya_ble_status_t tuya_ble_gap_disconnect(void);
tuya_ble_status_t tuya_ble_gap_addr_get(tuya_ble_gap_addr_t* p_addr);
tuya_ble_status_t tuya_ble_gap_addr_set(tuya_ble_gap_addr_t* p_addr);
tuya_ble_status_t tuya_ble_gatt_send_data(const uint8_t* p_data, uint16_t len);
tuya_ble_status_t tuya_ble_common_uart_init(void);
tuya_ble_status_t tuya_ble_common_uart_send_data(const uint8_t* p_data, uint16_t len);
tuya_ble_status_t tuya_ble_timer_create(void** p_timer_id, uint32_t timeout_value_ms, tuya_ble_timer_mode mode, tuya_ble_timer_handler_t timeout_handler);
tuya_ble_status_t tuya_ble_timer_delete(void* timer_id);
tuya_ble_status_t tuya_ble_timer_start(void* timer_id);
tuya_ble_status_t tuya_ble_timer_restart(void* timer_id,uint32_t timeout_value_ms);
tuya_ble_status_t tuya_ble_timer_stop(void* timer_id);
void tuya_ble_device_delay_ms(uint32_t ms);
tuya_ble_status_t tuya_ble_rand_generator(uint8_t* p_buf, uint8_t len);
tuya_ble_status_t tuya_ble_device_reset(void);
tuya_ble_status_t tuya_ble_rtc_get_timestamp(uint32_t* timestamp, int32_t* timezone);
tuya_ble_status_t tuya_ble_rtc_set_timestamp(uint32_t timestamp, int32_t timezone);
tuya_ble_status_t tuya_ble_nv_init(void);
tuya_ble_status_t tuya_ble_nv_erase(uint32_t addr, uint32_t size);
tuya_ble_status_t tuya_ble_nv_write(uint32_t addr, const uint8_t* p_data, uint32_t size);
tuya_ble_status_t tuya_ble_nv_read(uint32_t addr, uint8_t* p_data, uint32_t size);


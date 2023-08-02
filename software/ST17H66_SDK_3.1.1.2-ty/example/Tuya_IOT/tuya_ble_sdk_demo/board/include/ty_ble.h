/**
****************************************************************************
* @file      ty_ble.h
* @brief     ty_ble
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_BLE_H__
#define __TY_BLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "board.h"

/*********************************************************************
 * CONSTANT
 */

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
typedef struct {
    uint32_t len;
    uint8_t  value[31];
} ty_ble_data_t;

typedef struct {
    double  adv_interval_min; //ms
    double  adv_interval_max; //ms
    uint8_t adv_type;
    uint8_t adv_power;
    uint8_t adv_channal_map;
} ty_ble_adv_param_t;

typedef struct {
    uint8_t type;
    uint8_t addr[6];
} ty_ble_mac_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */
#if (!CHIP_PLATFORM_NRF52832)
extern ty_ble_data_t        s_adv_data;
extern ty_ble_data_t        s_scan_rsp;
extern ty_ble_adv_param_t   s_adv_param;
#endif

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_ble_init(void);
uint32_t ty_ble_start_adv(void);
uint32_t ty_ble_restart_adv(void);
uint32_t ty_ble_stop_adv(void);
uint32_t ty_ble_set_advdata_and_scanrsp(const ty_ble_data_t* p_adv_data, const ty_ble_data_t* p_scan_rsp);
uint32_t ty_ble_set_adv_param(const ty_ble_adv_param_t* p_param);
uint32_t ty_ble_connect(const ty_ble_mac_t* p_mac);
uint32_t ty_ble_reconnect(const ty_ble_mac_t* p_mac);
uint32_t ty_ble_disconnect(void);
uint32_t ty_ble_set_conn_param(uint16_t cMin, uint16_t cMax, uint16_t latency, uint16_t timeout);
uint32_t ty_ble_set_mac(const ty_ble_mac_t* p_mac);
uint32_t ty_ble_get_mac(ty_ble_mac_t* p_mac);
uint32_t ty_ble_connect_handler(void);
uint32_t ty_ble_disconnect_handler(void);
uint32_t ty_ble_receive_data_handler(const uint8_t* buf, uint32_t size);
uint32_t ty_ble_send_data(const uint8_t* buf, uint32_t size);
uint32_t ty_ble_get_rssi(int8_t* p_rssi);
uint32_t ty_ble_set_tx_power(int8_t tx_power);
uint32_t ty_ble_set_device_name(const uint8_t* buf, uint16_t size);
uint32_t ty_ble_get_device_name(uint8_t* buf, uint16_t* p_size);
uint32_t ty_ble_set_dle(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_BLE_H__

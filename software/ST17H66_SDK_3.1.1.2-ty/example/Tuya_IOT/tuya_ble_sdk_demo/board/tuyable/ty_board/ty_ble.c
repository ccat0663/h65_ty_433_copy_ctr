#include "ty_ble.h"
#include "tuya_ble_api.h"
#include "tuya_ble_ota.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_sdk_demo.h"
#include "tuya_ble_service.h"
#include "bcomdef.h"
#include "peripheral.h"
#include "simpleBLEPeripheral.h"
#include "ll.h"
#include "gap.h"
#include "gapgattserver.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define     DEFAULT_ADV_DATA    \
            {                   \
                3,              \
                {               \
                    0x02,       \
                    0x01,       \
                    0x06,       \
                },              \
            }
            
#define     DEFAULT_SCAN_RSP    \
            {                   \
                13,              \
                {               \
                    0x0C,       \
                    0x09,       \
                    't', 'y', '_', 'b', 'l', 'e', '_', 'd', 'e', 'm', 'o', \
                },              \
            }

#define     DEFAULT_ADV_PARAM               \
            {                               \
                .adv_interval_min = TY_ADV_INTERVAL,    \
                .adv_interval_max = TY_ADV_INTERVAL,    \
                .adv_type         = 0x01,   \
                .adv_power        = 0x00,   \
                .adv_channal_map  = 0x07,   \
            }

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
ty_ble_data_t        s_adv_data  = DEFAULT_ADV_DATA;
ty_ble_data_t        s_scan_rsp  = DEFAULT_SCAN_RSP;
ty_ble_adv_param_t   s_adv_param = DEFAULT_ADV_PARAM;

/*********************************************************************
 * VARIABLE
 */
volatile bool g_is_restart_adv = false;

/*********************************************************************
 * LOCAL FUNCTION
 */






/*********************************************************
FN: 
*/
uint32_t ty_ble_init(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_start_adv(void)
{
    if((g_gap_state == GAPROLE_STARTED) 
        || (g_gap_state == GAPROLE_WAITING) 
        || g_gap_state == GAPROLE_WAITING_AFTER_TIMEOUT) {
        uint8_t adv_enable = TRUE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &adv_enable);
        
        TY_PRINTF("adv start");
        return 0;
    } else {
        TY_PRINTF("adv start invalid state");
        return 1;
    }
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_restart_adv(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_stop_adv(void)
{
    if(g_gap_state == GAPROLE_ADVERTISING) {
        uint8_t adv_enable = FALSE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &adv_enable);
        
        TY_PRINTF("adv stop");
        return 0;
    } else {
//        TY_PRINTF("adv start invalid state");
        return 1;
    }
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_advdata_and_scanrsp(const ty_ble_data_t* p_adv_data, const ty_ble_data_t* p_scan_rsp)
{
    if(ty_ble_stop_adv() == 0) {
        g_is_restart_adv = true;
    }

    if(p_adv_data != NULL) {
        if(p_adv_data->len > 31) {
            return PPlus_ERR_DATA_SIZE;
        } else {
            s_adv_data.len = p_adv_data->len;
            osal_memcpy(s_adv_data.value, p_adv_data->value, p_adv_data->len);
            GAPRole_SetParameter(GAPROLE_ADVERT_DATA, s_adv_data.len, s_adv_data.value);
        }
    }
    if(p_scan_rsp != NULL) {
        if(p_scan_rsp->len > 31) {
            return PPlus_ERR_DATA_SIZE;
        } else {
            s_scan_rsp.len = p_scan_rsp->len;
            osal_memcpy(s_scan_rsp.value, p_scan_rsp->value, p_scan_rsp->len);
            GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, s_scan_rsp.len, s_scan_rsp.value);
        }
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_adv_param(const ty_ble_adv_param_t* p_param)
{
    if(ty_ble_stop_adv() == 0) {
        g_is_restart_adv = true;
    }

    uint16 adv_interval_max = p_param->adv_interval_max*8/5;
    uint16 adv_interval_min = p_param->adv_interval_min*8/5;
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, adv_interval_min);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, adv_interval_max);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, adv_interval_min);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, adv_interval_max);
    
    uint8 advType = LL_ADV_CONNECTABLE_UNDIRECTED_EVT;
    GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof( uint8 ), &advType);
    
    uint8 advChnMap = GAP_ADVCHAN_37 | GAP_ADVCHAN_38 | GAP_ADVCHAN_39;
    GAPRole_SetParameter(GAPROLE_ADV_CHANNEL_MAP, sizeof(uint8), &advChnMap);
    
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_connect(const ty_ble_mac_t* p_mac)
{
    return TUYA_BLE_SUCCESS;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_reconnect(const ty_ble_mac_t* p_mac)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_disconnect(void)
{
    return GAPRole_TerminateConnection();
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_conn_param(uint16_t cMin, uint16_t cMax, uint16_t latency, uint16_t timeout)
{
//    uint8 enable_update_request = TRUE;
    uint16 desired_min_interval = cMin*4/5;
    uint16 desired_max_interval = cMax*4/5;
    uint16 desired_slave_latency = latency;
    uint16 desired_conn_timeout = timeout/10;
    uint8  update_request = TRUE;
//    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_REQ, sizeof( uint8 ), &update_request );
    return TUYA_BLE_SUCCESS;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_mac(const ty_ble_mac_t* p_mac)
{
    if(ty_ble_stop_adv() == 0) {
        g_is_restart_adv = true;
    }
    
    volatile uint8_t* p_ownPublicAddr = (volatile uint8_t*)0x1fff0965;  // It's 0x1fff0965 for 6222.
    memcpy((void*)p_ownPublicAddr, p_mac->addr, 6);
    
    return TUYA_BLE_SUCCESS;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_get_mac(ty_ble_mac_t* p_mac)
{
    volatile uint8_t* p_ownPublicAddr = (volatile uint8_t*)0x1fff0965;  // It's 0x1fff0965 for 6222.
    memcpy(p_mac->addr, (void*)p_ownPublicAddr, 6);
    return TUYA_BLE_SUCCESS;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_connect_handler(void)
{
    tuya_ble_connected_handler();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_disconnect_handler(void)
{
    tuya_ble_disconnected_handler();
    //tuya_ble_ota_disconn_handler();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_receive_data_handler(const uint8_t* buf, uint32_t size)
{
    TY_PRINTF("ble rx size: %d", size);

    tuya_ble_gatt_receive_data((void*)buf, size);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_send_data(const uint8_t* buf, uint32_t size)
{
		TY_PRINTF("ble tx size: %d", size);
    return ty_ble_notify((void*)buf, size);
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_get_rssi(int8_t* p_rssi)
{
    //peripheralStateReadRssiCB
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_tx_power(int8_t tx_power)
{
    //g_rfPhyTxPower
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_device_name(const uint8_t* buf, uint16_t size)
{
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, size, (void*)buf);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_get_device_name(uint8_t* buf, uint16_t* p_size)
{
    *p_size = GAP_DEVICE_NAME_LEN;
    GGS_GetParameter(GGS_DEVICE_NAME_ATT, (void*)buf);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_ble_set_dle(void)
{
    return 0;
}







/**
****************************************************************************
* @file      tuya_ble_ota.h
* @brief     tuya_ble_ota
* @author    suding
* @version   V1.0.0
* @date      2019-09-11
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2019 Tuya </center></h2>
*/


#ifndef __TUYA_BLE_OTA_H__
#define __TUYA_BLE_OTA_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "stdint.h"
#include "tuya_ble_type.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
uint32_t tuya_ble_ota_init(void);
void tuya_ble_ota_handler(tuya_ble_ota_data_t* ota);
uint32_t tuya_ble_ota_get_state(void);
uint32_t tuya_ble_ota_disconn_handler(void);


#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_OTA_H__

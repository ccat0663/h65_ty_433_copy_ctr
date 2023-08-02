/**
****************************************************************************
* @file      tuya_ble_service.h
* @brief     tuya_ble_service
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_BLE_SERVICE_H__
#define __TY_BLE_SERVICE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "bcomdef.h"

/*********************************************************************
 * CONSTANT
 */
enum {
    TUYA_EVT_NOTI_DISABLED = 1,
    TUYA_EVT_NOTI_ENABLED,
    TUYA_EVT_BLE_DATA_RECIEVED,
};

/*********************************************************************
 * STRUCT
 */
typedef struct {
    uint8_t  event;
    uint8_t  size;
    uint8_t* data;
} tuya_svc_event_t;

typedef void (*tuya_ProfileChangeCB_t)(tuya_svc_event_t* pev);

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
bStatus_t tuya_gen_AddService(void);
uint8 tuya_NotifyIsReady(void);
bStatus_t ty_ble_notify(uint8_t* value, uint32 size);
void set_tuya_Notify(void);
void clear_tuya_Notify(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_BLE_SERVICE_H__

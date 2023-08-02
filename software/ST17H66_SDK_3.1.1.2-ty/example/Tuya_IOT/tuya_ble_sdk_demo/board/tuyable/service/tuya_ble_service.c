#include "tuya_ble_service.h"
#include "tuya_ble_type.h"
#include "custom_tuya_ble_config.h"
#include "ty_ble.h"

#include "bcomdef.h"
#include <stdio.h>
#include <string.h>
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gatt_profile_uuid.h"
#include "peripheral.h"
#include "gattservapp.h"
#include "clock.h"
#include "ota_app_service.h"
#include "log.h"
#include "error.h"
#include "ll.h"
#include "simpleBLEPeripheral.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define TY_BLE_SERVICE_UUID         0xFD50

#define TY_BLE_WRITE_HANDLE         2
#define TY_BLE_NOTIFY_HANDLE        4

CONST uint8 tuya_charUserDescUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(GATT_CHAR_USER_DESC_UUID), HI_UINT16(GATT_CHAR_USER_DESC_UUID)
};

CONST uint8 tuya_gen_ServiceUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(TY_BLE_SERVICE_UUID), HI_UINT16(TY_BLE_SERVICE_UUID)
};

CONST uint8 tuya_charNotiUUID[ATT_UUID_SIZE] = {
    0xD0, 0x07, 0x9B, 0x5F, 0x80, 0x00, 0x01, 0x80,
    0x01, 0x10, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
};

CONST uint8 tuya_charWritUUID[ATT_UUID_SIZE] = {
    0xD0, 0x07, 0x9B, 0x5F, 0x80, 0x00, 0x01, 0x80,
    0x01, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};
            
/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
//tuya general service
static CONST gattAttrType_t tuya_gen_Service = {ATT_BT_UUID_SIZE, tuya_gen_ServiceUUID};
//tuya Notify characteristic
static uint8 tuya_charNotiProps = GATT_PROP_NOTIFY;
static uint8 tuya_charNotiValue[TUYA_BLE_DATA_MTU_MAX] = {0}; // tbd
const uint8 tuya_charNotiDescipt[] = "Dev2APP"; // tbd

// tuya Write characteristic
static uint8 tuya_charWritProps =  GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;
static uint8 tuya_charWritValue[TUYA_BLE_DATA_MTU_MAX] = {0}; // tbd
const uint8 tuya_charWritDescipt[] = "APP2Dev"; // tbd

static gattCharCfg_t tuya_ResponseCCCD[GATT_MAX_NUM_CONN];

tuya_svc_event_t tuya_svc_event = {0};
tuya_ble_evt_param_t event = {0};

static gattAttribute_t tuya_gen_AttrTbl[] =
{
    //tuya general Service
    {
        {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
        GATT_PERMIT_READ,                       /* permissions */
        0,                                      /* handle */
        (uint8*)& tuya_gen_Service              /* pValue */
    },

    //tuya Write characteristic Declaration
    {
        {ATT_BT_UUID_SIZE, characterUUID},
        GATT_PERMIT_READ,
        0,
        &tuya_charWritProps
    },

    //tuya Write characteristic Value
    {
        //{ATT_BT_UUID_SIZE, tuya_charWritUUID},
			  {ATT_UUID_SIZE, tuya_charWritUUID},
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)(&tuya_charWritValue)
    },
    
    
    //tuya Notify characteristic Declaration
    {
        {ATT_BT_UUID_SIZE, characterUUID},
        GATT_PERMIT_READ,
        0,
        &tuya_charNotiProps
    },

    //tuya Notify characteristic Value  
    {
        //{ATT_BT_UUID_SIZE, tuya_charNotiUUID},
			  {ATT_UUID_SIZE, tuya_charNotiUUID},
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)(&tuya_charNotiValue)
    },

    //tuya response Client Characteristic Configuration
    {
        {ATT_BT_UUID_SIZE, clientCharCfgUUID},
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)(tuya_ResponseCCCD)
    },
};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */
static uint8 ty_read_attr_cb(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint16* pLen, uint16 offset, uint8 maxLen);
static bStatus_t ty_write_attr_cb(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint16 len, uint16 offset);

CONST gattServiceCBs_t ty_profile_cbs = {
    (pfnGATTReadAttrCB_t)ty_read_attr_cb,
    (pfnGATTWriteAttrCB_t)ty_write_attr_cb,
    NULL,
};




/*********************************************************
FN: 
*/
uint8 tuya_NotifyIsReady(void)
{
//    return (tuya_ResponseCCCD.value == GATT_CLIENT_CFG_NOTIFY);
	return 1;
}

/*********************************************************
FN: 
*/
void set_tuya_Notify(void)
{
//    tuya_ResponseCCCD.value |= GATT_CLIENT_CFG_NOTIFY;
}

/*********************************************************
FN: 
*/
void clear_tuya_Notify(void)
{
//    tuya_ResponseCCCD.value = 0;
}

/*********************************************************
FN: Validate attribute data before a write operation
PM: connHandle - connection message was received on
    pAttr - pointer to attribute
    pValue - pointer to data to be written
    len - length of data
    offset - offset of the first octet to be written
*/
static bStatus_t ty_write_attr_cb(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint16 len, uint16 offset)
{
    bStatus_t status = SUCCESS;

    // If attribute permissions require authorization to write, return error
    if(gattPermitAuthorWrite(pAttr->permissions)) {
        // Insufficient authorization
        return (ATT_ERR_INSUFFICIENT_AUTHOR);
    }

    if(osal_memcmp(pAttr->type.uuid, clientCharCfgUUID, 2)) {
        // case:GATT_CLIENT_CHAR_CFG_UUID(0x2902)
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY);

        if (status == SUCCESS) {
            uint16 charCfg = BUILD_UINT16(pValue[0], pValue[1]);
            
//            TY_PRINTF("CCCD set: [%d]", charCfg);
            
            if(charCfg == GATT_CFG_NO_OPERATION) {
                clear_tuya_Notify();
            } else {
                set_tuya_Notify();
            }
        }
    } else if(osal_memcmp(pAttr->type.uuid, tuya_charWritUUID, ATT_UUID_SIZE) 
            && (pAttr->handle == tuya_gen_AttrTbl[TY_BLE_WRITE_HANDLE].handle)) {
        ty_ble_receive_data_handler((uint8_t*)pValue, len);
    } else {
        return (ATT_ERR_ATTR_NOT_FOUND);
    }

    return (status);
}

/*********************************************************
FN: 
PM: connHandle - connection message was received on
    pAttr - pointer to attribute
    pValue - pointer to data to be read
    pLen - length of data to be read
    offset - offset of the first octet to be read
    maxLen - maximum length of data to be read
*/
static uint8 ty_read_attr_cb(uint16 connHandle, gattAttribute_t* pAttr, uint8* pValue, uint16* pLen, uint16 offset, uint8 maxLen)
{
    bStatus_t status = SUCCESS;

    // If attribute permissions require authorization to read, return error
    if( gattPermitAuthorRead(pAttr->permissions)) {
        // Insufficient authorization
        return (ATT_ERR_INSUFFICIENT_AUTHOR);
    }

    if(osal_memcmp(pAttr->type.uuid, clientCharCfgUUID, 2)) {
        *pLen = sizeof(gattCharCfg_t);
        
        osal_memcpy(pValue, (uint8 *)&tuya_ResponseCCCD, *pLen);
        
        TY_PRINTF("111: %d", tuya_ResponseCCCD->value);
    } else if(osal_memcmp(pAttr->type.uuid, tuya_charNotiUUID, ATT_UUID_SIZE)) {
        *pLen = TUYA_BLE_DATA_MTU_MAX;
        
        osal_memcpy(pValue, tuya_charNotiValue, *pLen);
        
        TY_PRINTF("222");
    } else {
        return (ATT_ERR_ATTR_NOT_FOUND);
    }
    
    return (status);
}

/*********************************************************
FN: 
*/
bStatus_t ty_ble_notify(uint8_t* value, uint32 size)
{
	uint16 connHandle;
    bStatus_t ret = SUCCESS;
    
    GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connHandle);
	if (connHandle == INVALID_CONNHANDLE) {
        return bleIncorrectMode;
    }
    
    if(size > ATT_GetCurrentMTUSize(connHandle)) {
        TY_PRINTF("ty_ble_notify bleMemAllocError");
        return bleMemAllocError;
    }

    uint16 notfEnable = GATTServApp_ReadCharCfg(0, tuya_ResponseCCCD);
    if (notfEnable & GATT_CLIENT_CFG_NOTIFY) {
        attHandleValueNoti_t pNoti = {0};
        pNoti.handle = tuya_gen_AttrTbl[TY_BLE_NOTIFY_HANDLE].handle;
        VOID osal_memcpy(pNoti.value, value, size);
        pNoti.len = size;
				TY_PRINTF("send:");
				LOG_DUMP_BYTE(pNoti.value,pNoti.len);
        ret = GATT_Notification(connHandle, &pNoti, FALSE);
    } else {
        ret = bleNotReady;
    }
    
    return ret;
}

/*********************************************************
FN: 
*/
static void ty_conn_status_cb(uint16 connHandle, uint8 changeType)
{
    if(connHandle != LOOPBACK_CONNHANDLE) {
        // Reset Client Char Config if connection has dropped
        if((changeType == LINKDB_STATUS_UPDATE_REMOVED )
            || ((changeType == LINKDB_STATUS_UPDATE_STATEFLAGS) && (!linkDB_Up(connHandle))))
        {
            GATTServApp_InitCharCfg(connHandle, tuya_ResponseCCCD);
        }
    }
}

/*********************************************************
FN: 
*/
bStatus_t tuya_gen_AddService(void)
{
    uint8 ret = SUCCESS;
    // Register with Link DB to receive link status change callback
    VOID linkDB_Register(ty_conn_status_cb);

	// Register GATT attribute list and CBs with GATT Server App
    ret = GATTServApp_RegisterService(tuya_gen_AttrTbl, GATT_NUM_ATTRS(tuya_gen_AttrTbl), &ty_profile_cbs);
    if(ret != SUCCESS) {
        LOG("fail to add tuya service ");
    }
    
    return ret;
}










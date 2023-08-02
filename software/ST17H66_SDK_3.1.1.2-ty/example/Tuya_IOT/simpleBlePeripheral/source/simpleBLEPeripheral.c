/**************************************************************************************************
*******
**************************************************************************************************/

/**************************************************************************************************
  Filename:       simpleBLEPeripheral.c
  Revised:        
  Revision:       

  Description:    This file contains the Simple BLE Peripheral sample application
                  

**************************************************************************************************/
/*********************************************************************
    INCLUDES
*/
#include "bcomdef.h"
#include "rf_phy_driver.h"
#include "global_config.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
//#include "devinfoservice.h"
//#include "sbpProfile_ota.h"
#include "ota_app_service.h"
#include "peripheral.h"
#include "gapbondmgr.h"
#include "pwrmgr.h"
#include "gpio.h"
#include "simpleBLEPeripheral.h"
#include "ll.h"
#include "ll_hw_drv.h"
#include "ll_def.h"
#include "hci_tl.h"
#include "gap.h"
#include "tuya_ble_service.h"

// start - header files for tuya ble sdk.
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_heap.h"
#include "tuya_ble_mem.h"
#include "tuya_ble_api.h"
#include "tuya_ble_port.h"
#include "tuya_ble_main.h"
#include "tuya_ble_secure.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_event_handler.h"
#include "tuya_ble_storage.h"
#include "tuya_ble_sdk_version.h"
#include "tuya_ble_utils.h"
#include "tuya_ble_event.h"
#include "tuya_ble_log.h"
#include "tuya_ble_sdk_demo.h"
#include "ty_ble.h"
#include "ty_system.h"
//#include "tuya_ble_sdk_test.h"
#include "osal_cbTimer.h"
#include "ty_key_press.h"
#include "led_driver.h"


/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static uint8_t s_device_name[] = TY_DEVICE_NAME;//"Demo";

/*********************************************************************
 * VARIABLE
 */
uint8 simpleBLEPeripheral_TaskID;
uint8 phy_timer_TaskID;
volatile gaprole_States_t g_gap_state = GAPROLE_INIT;

/*********************************************************************
 * LOCAL FUNCTION
 */
static void param_update_CB(uint16 connInterval, uint16 connSlaveLatency, uint16 connTimeout);
static void peripheralStateNotificationCB(gaprole_States_t newState);
static void peripheralStateReadRssiCB(int8 rssi);

// GAP Role Callbacks
static gapRolesCBs_t simpleBLEPeripheral_PeripheralCBs = {
    peripheralStateNotificationCB,  // Profile State Change Callbacks
    peripheralStateReadRssiCB       // When a valid RSSI is read from controller (not used by application)
};
static gapRolesParamUpdateCB_t param_update_CB_fun = param_update_CB;




/*********************************************************
FN: 
*/
void param_update_CB(uint16 connInterval, uint16 connSlaveLatency, uint16 connTimeout)
{
    TY_PRINTF("conn param update: connInterval-%dms, latency-%d, timeout-%dms", \
        (uint16_t)(connInterval*1.25),  \
        (uint16_t)(connSlaveLatency),   \
        (uint16_t)(connTimeout*10) );
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_Init
 *
 * @brief   Initialization function for the Simple BLE Peripheral App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SimpleBLEPeripheral_Init(uint8 task_id)
{
    simpleBLEPeripheral_TaskID = task_id;

    
    VOID GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, 6);

    ty_ble_set_device_name(s_device_name, sizeof(s_device_name));
  
    ty_ble_set_conn_param(TY_CONN_INTERVAL_MIN, TY_CONN_INTERVAL_MAX, 0, 6000);

    ty_ble_adv_param_t param;
    param.adv_interval_min = s_adv_param.adv_interval_min;
    param.adv_interval_max = s_adv_param.adv_interval_max;
    ty_ble_set_adv_param(&param);
    
    
    //add service
    GGS_AddService(GATT_ALL_SERVICES);          // GAP
    GATTServApp_AddService(GATT_ALL_SERVICES);  // GATT attributes
    tuya_gen_AddService();                      // Device Information Service  Harris 12-30

    //set gatt param
    llInitFeatureSet2MPHY(TRUE);
    llInitFeatureSetDLE(TRUE);
    ATT_SetMTUSizeMax(247);
    HCI_LE_SetDefaultPhyMode(0, 0, 0x01, 0x01);

    // Setup a delayed profile startup
    osal_set_event(simpleBLEPeripheral_TaskID, SBP_START_DEVICE_EVT);
    // for receive HCI complete message
    GAP_RegisterForHCIMsgs(simpleBLEPeripheral_TaskID);

    GAPRole_RegisterAppCBs(&param_update_CB_fun);
}

/*********************************************************************
 * @fn
 */
void phy_timer_init(uint8 task_id)
{
    phy_timer_TaskID = task_id;

    ty_system_init(0);
    
    tuya_ble_sdk_demo_init();

#if TUYA_BLE_SDK_TEST
    tuya_ble_sdk_test_init();
#endif

//    ty_mainloop_timer_start();
}

/*********************************************************************
 * @fn      simpleBLEPeripheral_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void simpleBLEPeripheral_ProcessOSALMsg(osal_event_hdr_t *pMsg)
{
    switch (pMsg->event)
    {
        case HCI_GAP_EVENT_EVENT: {
            switch(pMsg->status)
            {
                case HCI_COMMAND_COMPLETE_EVENT_CODE: {
                } break;

                default: {
                    //safeToDealloc = FALSE;  // Send to app
                } break;
            }
        }
      }
}

/*********************************************************************
 * @fn      SimpleBLEPeripheral_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 SimpleBLEPeripheral_ProcessEvent(uint8 task_id, uint16 events)
{

    if(events & SYS_EVENT_MSG) {
        uint8 *pMsg;
        if((pMsg = osal_msg_receive(simpleBLEPeripheral_TaskID)) != NULL) {
            simpleBLEPeripheral_ProcessOSALMsg((osal_event_hdr_t *)pMsg);

            VOID osal_msg_deallocate(pMsg);
        }
        return (events^SYS_EVENT_MSG);
    }

    if(events & SBP_START_DEVICE_EVT) {
        // Start the Device
        VOID GAPRole_StartDevice(&simpleBLEPeripheral_PeripheralCBs);
        HCI_LE_ReadResolvingListSizeCmd();
        return (events^SBP_START_DEVICE_EVT);
    }
		
    if(events & SBP_RESET_ADV_EVT) {
        ty_ble_start_adv();
        return (events^SBP_RESET_ADV_EVT);
    }

    if(events & SBP_TUYA_BLE_CONN_EVT) {
        TY_PRINTF("Connected");
        ty_ble_connect_handler();
        
        return ( events^SBP_TUYA_BLE_CONN_EVT );
    }

    if(events & SBP_TUYA_BLE_DISCONN_EVT) {
        TY_PRINTF("Disconnect");
        ty_ble_disconnect_handler();
        
        return (events^SBP_TUYA_BLE_DISCONN_EVT);
    }

    if(events & SBP_TUYA_EXEC_EVT) {
				//TY_PRINTF("-");
        tuya_ble_main_tasks_exec();

        //设置led灯状态
        switch(tuya_ble_connect_status_get())
        {
            case UNBONDING_UNCONN:
            case BONDING_CONN:

                //未绑定或者连接时常亮
                led_set_status(LED_BLUE, LED_BLE_CONNECT);

                break;
            default:

                //其他状态下常灭
                led_reset_status(LED_BLUE, LED_BLE_CONNECT);

                break;
        }

        return (events^SBP_TUYA_EXEC_EVT);
    }

    if(events & SBP_RTC_TEST_EVT) {
        void ty_rtc_handler(void);
        ty_rtc_handler();
        return (events^SBP_RTC_TEST_EVT);
    }

    if(events & SBP_TY_KEY_EVT) {
        extern ty_key_press_param_t ty_key_press_param;
        ty_key_press_timeout_handler(&ty_key_press_param);
        return (events^SBP_TY_KEY_EVT);
    }

    return 0;
}

/*********************************************************************
 * @fn
 */
uint16 phy_timer_ProcessEvent(uint8 task_id, uint16 events)
{

    uint16 phy_timer_handler(uint16_t timer_id);
    return phy_timer_handler(events);
}

/*********************************************************************
 * @fn      peripheralStateNotificationCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateNotificationCB(gaprole_States_t newState)
{
    gaprole_States_t g_gap_state_last = g_gap_state;
    g_gap_state = newState;
    
    switch (newState)
    {
        case GAPROLE_STARTED: {
			HCI_ReadBDADDRCmd();
			osal_set_event(simpleBLEPeripheral_TaskID, SBP_RESET_ADV_EVT);
        } break;
        
        case GAPROLE_ADVERTISING: {
         } break;
        
        case GAPROLE_CONNECTED: {
            osal_set_event(simpleBLEPeripheral_TaskID, SBP_TUYA_BLE_CONN_EVT);
        } break;
        
        case GAPROLE_WAITING:
        case GAPROLE_WAITING_AFTER_TIMEOUT: {
            if(g_gap_state_last == GAPROLE_CONNECTED) {
                osal_set_event(simpleBLEPeripheral_TaskID, SBP_TUYA_BLE_DISCONN_EVT);
            }
            else if(g_gap_state_last == GAPROLE_ADVERTISING) {
                extern volatile bool g_is_restart_adv;
                if(g_is_restart_adv) {
                    g_is_restart_adv = false;
                    ty_ble_start_adv();
                }
            }
        } break;
        
        default: {
        } break;
    }
}

/*********************************************************************
 * @fn      peripheralStateReadRssiCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void peripheralStateReadRssiCB(int8 rssi)
{
    TY_PRINTF("rssi: %d", rssi);
}


/*********************************************************
FN: 
*/
void ty_mainloop_timer_start(void)
{
    osal_start_reload_timer(simpleBLEPeripheral_TaskID, SBP_TUYA_EXEC_EVT, 20);
}

/*********************************************************
FN: 
*/
void ty_mainloop_timer_stop(void)
{
    osal_stop_timerEx(simpleBLEPeripheral_TaskID, SBP_TUYA_EXEC_EVT);
}








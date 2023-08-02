/**************************************************************************************************
*******
**************************************************************************************************/

/**************************************************************************************************
    Filename:       OSAL_SimpleBLEPeripheral.c
    Revised:
    Revision:

    Description:    This file contains function that allows user setup tasks



**************************************************************************************************/

/**************************************************************************************************
                                              INCLUDES
 **************************************************************************************************/
#if (APP_CFG == 0)
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "osal_cbTimer.h"

/* LL */
#include "ll.h"

/* HCI */
#include "hci_tl.h"

#if defined ( OSAL_CBTIMER_NUM_TASKS )
    #include "osal_cbTimer.h"
#endif

/* L2CAP */
#include "l2cap.h"

/* gap */
#include "gap.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

/* GATT */
#include "gatt.h"

#include "gattservapp.h"

/* Profiles */
#include "peripheral.h"

/* Application */
#include "simpleBLEPeripheral.h"
#include "ty_mcu_sdk_app.h"
#include "ty_dev_config.h"
#include "led_driver.h"
#include "key_driver.h"
#include "rc_task.h"
#include "app_ctr_task.h"
/*********************************************************************
    GLOBAL VARIABLES
*/

// The order in this table must be identical to the task initialization calls below in osalInitTask.
const pTaskEventHandlerFn tasksArr[] =
{
    LL_ProcessEvent,                                                  // task 0

    //涂鸦纯ble功能
    #if(TY_BLE_SDK == 1)
    HCI_ProcessEvent,                                                 // task 1
    #if defined ( OSAL_CBTIMER_NUM_TASKS )
    OSAL_CBTIMER_PROCESS_EVENT( osal_CbTimerProcessEvent ),           // task 3
    #endif
    L2CAP_ProcessEvent,                                               // task 2
    SM_ProcessEvent,                                                  // task 3
    GAP_ProcessEvent,                                                 // task 4
    GATT_ProcessEvent,                                                // task 5
    GAPRole_ProcessEvent,                                             // task 6
    #if (DEF_GAPBOND_MGR_ENABLE==1)
    GAPBondMgr_ProcessEvent,                                          // task , add 2017-11-15
    #endif
    GATTServApp_ProcessEvent,                                         // task 7
    SimpleBLEPeripheral_ProcessEvent,                                 // task 8
    phy_timer_ProcessEvent,                                           // task 9

    //涂鸦MCU连接wbr3功能
    #else

    ty_mcu_sdk_app_event,

    #endif

    led_task_event,

    key_task_event,

    rc_task_event,

    app_ctr_task_event,

};

const uint8 tasksCnt = sizeof( tasksArr ) / sizeof( tasksArr[0] );
uint16* tasksEvents;

/*********************************************************************
    FUNCTIONS
 *********************************************************************/

/*********************************************************************
    @fn      osalInitTasks

    @brief   This function invokes the initialization function for each task.

    @param   void

    @return  none
*/
void osalInitTasks( void )
{
    uint8 taskID = 0;
    tasksEvents = (uint16*)osal_mem_alloc( sizeof( uint16 ) * tasksCnt);
    osal_memset( tasksEvents, 0, (sizeof( uint16 ) * tasksCnt));
    /* LL Task */
    LL_Init( taskID++ );

    #if(TY_BLE_SDK == 1)
    /* HCI Task */
    HCI_Init( taskID++ );
    #if defined ( OSAL_CBTIMER_NUM_TASKS )
    /* Callback Timer Tasks */
    osal_CbTimerInit( taskID );
    taskID += OSAL_CBTIMER_NUM_TASKS;
    #endif
    /* L2CAP Task */
    L2CAP_Init( taskID++ );
    /* SM Task */
    SM_Init( taskID++ );
    /* GAP Task */
    GAP_Init( taskID++ );
    /* GATT Task */
    GATT_Init( taskID++ );
    /* Profiles */
    GAPRole_Init( taskID++ );
    #if(DEF_GAPBOND_MGR_ENABLE==1)
    GAPBondMgr_Init( taskID++ );          // 2017-11-15
    #endif
    GATTServApp_Init( taskID++ );
    /* Application */
    SimpleBLEPeripheral_Init( taskID++ );
    /* Timer */
    phy_timer_init( taskID++ );

    #else

    ty_mcu_sdk_app_init( taskID++ );

    #endif

    led_task_init(taskID++);

    key_task_init(taskID++);

    rc_task_init(taskID++);

    app_ctr_task_init(taskID++);
}
#endif
/*********************************************************************
*********************************************************************/

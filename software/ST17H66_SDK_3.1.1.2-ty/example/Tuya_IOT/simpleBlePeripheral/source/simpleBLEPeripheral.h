/**************************************************************************************************
*******
**************************************************************************************************/

/**************************************************************************************************
  Filename:       simpleBLEperipheral.h
  Revised:         
  Revision:        

  Description:    This file contains the Simple BLE Peripheral sample application
                  definitions and prototypes.

 
**************************************************************************************************/

#ifndef SIMPLEBLEPERIPHERAL_H
#define SIMPLEBLEPERIPHERAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
    INCLUDES
*/
#include "types.h"
#include "bcomdef.h"
#include "peripheral.h"

/*********************************************************************
    CONSTANTS
*/
#define SBP_START_DEVICE_EVT                           0x0001
#define SBP_RESET_ADV_EVT                              0x0002
#define SBP_TUYA_BLE_CONN_EVT                          0x0004
#define SBP_TUYA_BLE_DISCONN_EVT                       0x0008
#define SBP_TUYA_EXEC_EVT                              0x0010
#define SBP_RTC_TEST_EVT                               0x0020
#define SBP_TY_KEY_EVT                                 0x0040

#define PHY_TIMER0_EVT                                 0x0001
#define PHY_TIMER1_EVT                                 0x0002
#define PHY_TIMER2_EVT                                 0x0004
#define PHY_TIMER3_EVT                                 0x0008
#define PHY_TIMER4_EVT                                 0x0010
#define PHY_TIMER5_EVT                                 0x0020
#define PHY_TIMER6_EVT                                 0x0040
#define PHY_TIMER7_EVT                                 0x0080
#define PHY_TIMER8_EVT                                 0x0100
#define PHY_TIMER9_EVT                                 0x0200
#define PHY_TIMER10_EVT                                0x0400
#define PHY_TIMER11_EVT                                0x0800
#define PHY_TIMER12_EVT                                0x1000
#define PHY_TIMER13_EVT                                0x2000
#define PHY_TIMER14_EVT                                0x4000
#define PHY_TIMER15_EVT                                0x8000

/*********************************************************************
 * MACROS
 */
extern uint8_t simpleBLEPeripheral_TaskID;
extern uint8_t phy_timer_TaskID;
extern volatile gaprole_States_t g_gap_state;

/*********************************************************************
    FUNCTIONS
*/

/*
    Task Initialization for the BLE Application
*/
extern void SimpleBLEPeripheral_Init( uint8 task_id );
extern void phy_timer_init(uint8 task_id);

/*
    Task Event Processor for the BLE Application
*/
extern uint16 SimpleBLEPeripheral_ProcessEvent( uint8 task_id, uint16 events );
extern uint16 phy_timer_ProcessEvent(uint8 task_id, uint16 events);

void ty_mainloop_timer_start(void);
void ty_mainloop_timer_stop(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBLEPERIPHERAL_H */

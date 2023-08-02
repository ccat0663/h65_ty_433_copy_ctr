#include "ty_rtc.h"
#include "bcomdef.h"
#include "OSAL_Timers.h"
#include "simpleBLEPeripheral.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static uint32_t s_local_timestamp = 0;
static uint32_t s_local_timestamp_when_update = 0;
static uint32_t s_app_timestamp_when_update = 0;

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
void ty_rtc_handler(void)
{
    s_local_timestamp++;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_init(void)
{
    osal_start_reload_timer(simpleBLEPeripheral_TaskID, SBP_RTC_TEST_EVT, 1000);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_set_time(uint32_t timestamp)
{
    s_local_timestamp_when_update = s_local_timestamp;
    s_app_timestamp_when_update = timestamp;
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_get_time(uint32_t* p_timestamp)
{
    *p_timestamp = (s_app_timestamp_when_update + (s_local_timestamp - s_local_timestamp_when_update));
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_get_local_time(uint32_t* p_timestamp)
{
    *p_timestamp = s_local_timestamp;
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_get_old_time(uint32_t old_local_timestamp, uint32_t* p_timestamp)
{
    uint32_t timestamp = 0;
    ty_rtc_get_time(&timestamp);
    *p_timestamp = (timestamp - (s_local_timestamp - old_local_timestamp));
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_control(uint8_t cmd, void* arg)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_rtc_uninit(void)
{
    osal_stop_timerEx(simpleBLEPeripheral_TaskID, SBP_RTC_TEST_EVT);
    return 0;
}








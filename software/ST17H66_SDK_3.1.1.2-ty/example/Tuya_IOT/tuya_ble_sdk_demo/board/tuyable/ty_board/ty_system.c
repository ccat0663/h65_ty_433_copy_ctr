#include "ty_system.h"
#include "ty_ble.h"
#include "ty_uart.h"
#include "ty_flash.h"
#include "ty_timer.h"
#include "ty_rtc.h"
#include "ty_pwm.h"
#include "ty_adc.h"
#include "ty_pin.h"
#include "ty_i2c.h"
#include "ty_spi.h"
#include "tuya_ble_api.h"
#include "tuya_ble_log.h"
//#include "tuya_ble_sdk_test.h"
#include "clock.h"
#include "ll_sleep.h"
#include "simpleBLEPeripheral.h"
#include "pwrmgr.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
            
/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
uint32_t ty_system_init(uint8_t location)
{
    switch(location)
    {
        case 0: {
            ty_system_log_init();
            //peripheral init
            ty_flash_init();
            //ty_uart2_init();
            //ty_rtc_init();
        } break;
        
        case 1: {
        } break;
        
        case 2: {
        } break;
        
        default: {
        } break;
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_mainloop(void)
{
    tuya_ble_main_tasks_exec();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_reset(void)
{
    hal_system_soft_reset();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_wdt_init(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_wdt_uninit(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_wdt_feed(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_log_init(void)
{
#if (TY_LOG_ENABLE||TUYA_BLE_LOG_ENABLE||TUYA_APP_LOG_ENABLE)
    elog_init();
//    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL);
    elog_start();
#endif
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size)
{
#if (TY_LOG_ENABLE||TUYA_BLE_LOG_ENABLE||TUYA_APP_LOG_ENABLE)
    elog_hexdump(name, width, buf, size);
#endif
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_delay_ms(uint32_t ms)
{
    WaitMs(ms);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_delay_us(uint32_t us)
{
    WaitUs(us);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_enter_sleep(void)
{
    if(hal_pwrmgr_is_lock(MOD_USR2)) {
        ty_mainloop_timer_stop();
        hal_pwrmgr_unlock(MOD_USR2);
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_system_exit_sleep(void)
{
    if(!hal_pwrmgr_is_lock(MOD_USR2)) {
        hal_pwrmgr_lock(MOD_USR2);
    }
    return 0;
}

/*********************************************************
FN: 
*/
bool ty_system_is_sleep(void)
{
    return 0;
}







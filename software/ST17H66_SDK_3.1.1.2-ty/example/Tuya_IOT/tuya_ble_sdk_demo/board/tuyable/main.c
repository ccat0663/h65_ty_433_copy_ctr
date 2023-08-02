#include "nrf_sdm.h"
#include "nrf_strerror.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "app_error.h"
#include "app_timer.h"
#include "ty_ble.h"
#include "ty_system.h"
#include "tuya_ble_log.h"
#include "tuya_ble_api.h"
#include "tuya_ble_sdk_test.h"




/*********************************************************************
 * LOCAL CONSTANTS
 */
//Value used as error code on stack dump, can be used to identify stack location on stack unwind.
#define DEAD_BEEF                       0xDEADBEEF




/*********************************************************
FN: 
*/
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
//    __disable_irq();

    switch (id)
    {
        case NRF_FAULT_ID_SD_ASSERT: {
            TUYA_APP_LOG_INFO("SOFTDEVICE: ASSERTION FAILED");
        } break;
        
        case NRF_FAULT_ID_APP_MEMACC: {
            TUYA_APP_LOG_INFO("SOFTDEVICE: INVALID MEMORY ACCESS");
        } break;
        
        case NRF_FAULT_ID_SDK_ASSERT: {
            assert_info_t * p_info = (assert_info_t *)info;
            TUYA_APP_LOG_INFO("ASSERTION FAILED at %s:%u",
                          p_info->p_file_name,
                          p_info->line_num);
            break;
        }
        
        case NRF_FAULT_ID_SDK_ERROR: {
            error_info_t * p_info = (error_info_t *)info;
            TUYA_APP_LOG_INFO("ERROR %u [%s] at %s:%u\r\nPC at: 0x%08x",
                          p_info->err_code,
                          nrf_strerror_get(p_info->err_code),
                          p_info->p_file_name,
                          p_info->line_num,
                          pc);
             TUYA_APP_LOG_INFO("End of error report");
            break;
        }
        
        default: {
            TUYA_APP_LOG_INFO("UNKNOWN FAULT at 0x%08X", pc);
        } break;
    }

//    SUBLE_PRINTF("System reset");
//    NRF_LOG_FINAL_FLUSH();
//    NVIC_SystemReset();
}

/*********************************************************
FN: Callback function for asserts in the SoftDevice.
    This function will be called in case of an assert in the SoftDevice.

    @warning This handler is an example only and does not fit a final product.
             You need to analyze how your product is supposed to react in case of Assert.
    @warning On assert from the SoftDevice, the system can only recover on reset.

PM: line_num   Line number of the failing ASSERT call.
    file_name  File name of the failing ASSERT call.
*/
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/*********************************************************
FN: Function for initializing the nrf log module.
*/
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/*********************************************************
FN: Function for handling the idle state (main loop).
    If there is no pending log operation, then sleep until next the next event occurs.
*/
static void idle_state_handle(void)
{
    ty_system_mainloop();
    ty_system_wdt_feed();
    if ((NRF_LOG_PROCESS() == false) && (tuya_ble_sleep_allowed_check() == true))
    {
        nrf_pwr_mgmt_run();
    }
}

/*********************************************************
FN: 
*/
static void power_management_init(void)
{
    uint32_t ret;
    ret = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(ret);
}

/*********************************************************
FN: Function for application main entry.
*/
int main(void)
{
    log_init();
    app_timer_init();
    ty_system_init(0);
    
    //power manage init
    power_management_init();
    
    ty_ble_init();
    tuya_ble_sdk_demo_init();

#if TUYA_BLE_SDK_TEST
    tuya_ble_sdk_test_init();
#endif

//    ty_system_wdt_init();
    ty_ble_start_adv();
    
    for (;;)
    {
        idle_state_handle();
    }
}

































#include "ty_pin.h"
#include "ty_ble.h"
#include "ty_uart.h"
#include "ty_rtc.h"
#include "ty_pwm.h"
#include "ty_adc.h"
#include "ty_i2c.h"
//#include "tuya_ble_sdk_test.h"
#include "simpleBLEPeripheral.h"
#include "ty_key_press.h"
#include "OSAL_Timers.h"
#include "gpio.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define TY_PIN_NUM      P23

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
void ty_pin_handler(gpio_pin_e pin,gpio_polarity_e type)
{
    osal_start_reload_timer(simpleBLEPeripheral_TaskID, SBP_TY_KEY_EVT, 10);
}

/*********************************************************
FN: 
*/
uint32_t ty_pin_init(uint8_t pin, ty_pin_mode_t mode)
{
    hal_gpio_pull_set(TY_PIN_NUM, GPIO_PULL_UP);
    hal_gpioin_register(TY_PIN_NUM, NULL, ty_pin_handler);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pin_set(uint8_t pin, ty_pin_level_t level)
{
    if(level == TY_PIN_HIGH) {
    } else {
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pin_get(uint8_t pin, ty_pin_level_t* p_level)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pin_control(uint8_t pin, uint8_t cmd, void* arg)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pin_uninit(uint8_t pin, ty_pin_mode_t mode)
{
    return 0;
}




static void cpt_key_state_handler(uint32_t state);

/*********************************************************************
 * VARIABLE
 */
ty_key_press_param_t ty_key_press_param = {
    .pin = TY_PIN_NUM,
    .valid_level = TY_KEY_PRESS_LEVEL_LOW,
    .count_len = 3,
    .count_array = {5, 300, 700},
    .handler = cpt_key_state_handler,
};

/*********************************************************
FN: 
*/
uint32_t ty_key_press_get_pin_level(uint32_t pin)
{
    return hal_gpio_read(pin);
}

/*********************************************************
FN: 
*/
static void cpt_key_state_handler(uint32_t state)
{
//    TY_PRINTF("state: %d", state);
    switch(state)
    {
        case 1: { //single pressdown
#if TUYA_BLE_SDK_TEST
            tuya_ble_sdk_test_wake_up_handler();
#endif
            ty_mainloop_timer_start();
        } break;
        
        case 2: { //long pressdown
        } break;
        
        case 3: { //long-7s pressdown
        } break;
        
        case 5: { //single realease
        } break;
        
        case 6: { //realease between 3s~7s
        } break;
        
        default: {
        } break;
    }
    
    if(state >= ty_key_press_param.count_len) {
        osal_stop_timerEx(simpleBLEPeripheral_TaskID, SBP_TY_KEY_EVT);
    }
}

/*********************************************************
FN: 
*/
void cpt_key_press_init(void)
{
    ty_key_press_init(&ty_key_press_param);
}








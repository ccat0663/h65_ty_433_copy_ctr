#include "ty_pwm.h"
#include "pwm.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static pwm_ch_t phy_pwm_chn = {
    .pwmN         = PWM_CH0,
    .pwmPin       = P14,
    .pwmDiv       = PWM_CLK_DIV_64,
    .pwmMode      = PWM_CNT_UP,
    .pwmPolarity  = PWM_POLARITY_RISING,
    .cmpVal       = 0,
    .cntTopVal    = 256
};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
uint32_t ty_pwm_init(ty_pwm_t* p_pwm)
{
    hal_pwm_module_init();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pwm_start(ty_pwm_t* p_pwm)
{
    phy_pwm_chn.cntTopVal = 1000000L/p_pwm->freq;
    
    if(p_pwm->duty >= 100) {
        phy_pwm_chn.cmpVal = phy_pwm_chn.cntTopVal - 1;
    } else {
        phy_pwm_chn.cmpVal    = 10000L * (p_pwm->duty)/(p_pwm->freq);
    }
	
    hal_pwm_close_channel(PWM_CH0);
    hal_pwm_destroy(PWM_CH0);

    hal_gpio_pin_init(p_pwm->pin, IE);
    hal_gpio_pull_set(p_pwm->pin, WEAK_PULL_UP);

    hal_pwm_init(PWM_CH0, PWM_CLK_DIV_64, PWM_CNT_UP, p_pwm->polarity);
    hal_pwm_set_count_val(PWM_CH0, phy_pwm_chn.cmpVal, phy_pwm_chn.cntTopVal);
    hal_pwm_open_channel(PWM_CH0, phy_pwm_chn.pwmPin);

    hal_pwm_start();

    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pwm_stop(ty_pwm_t* p_pwm)
{
    hal_pwm_stop();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pwm_control(ty_pwm_t* p_pwm, uint8_t cmd, void* arg)
{
    switch(cmd) {
        case TY_PWM_CMD_SET_POLARITY: {
            ty_pwm_set_polarity_t* param = arg;
            p_pwm->polarity = param->polarity;
            ty_pwm_start(p_pwm);
        } break;
        
        case TY_PWM_CMD_SET_FREQ: {
            ty_pwm_set_freq_t* param = arg;
            p_pwm->polarity = param->freq;
            ty_pwm_start(p_pwm);
        } break;
        
        case TY_PWM_CMD_SET_DUTY: {
            ty_pwm_set_duty_t* param = arg;
            p_pwm->polarity = param->duty;
            ty_pwm_start(p_pwm);
        } break;
        
        default: {
        } break;
    }
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_pwm_uninit(ty_pwm_t* p_pwm)
{
    hal_pwm_module_deinit();
    return 0;
}








#include "ty_adc.h"
#include "adc.h"
#include "OSAL.h"
#include "log.h"
#include "ty_system.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define MAX_SAMPLE_POINT    64
#define PHY_ADC_PIN_IDX     4  // Notes: It MUST be updated accordingly in case changing channel.

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static uint16_t adc_debug[MAX_SAMPLE_POINT];
static uint8_t  channel_done_flag = 0; 
static uint16_t g_adc_value = 0;
 
static adc_Cfg_t phy_adc_cfg = {
    .channel = ADC_BIT(ADC_CH2N_P24),
    .is_continue_mode = FALSE,
    .is_differential_mode = 0x00,
    .is_high_resolution = 0x00,
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
static void adc_evt(adc_Evt_t* pev)
{
    float value = 0;
    int i = 0;
    bool is_high_resolution = FALSE;
    bool is_differential_mode = FALSE;
    uint8_t ch = 0;

    if((pev->type != HAL_ADC_EVT_DATA) || (pev->ch < 2)) {
        return;
    }

    osal_memcpy(adc_debug, pev->data, 2*(pev->size));
    channel_done_flag |= BIT(pev->ch);

    if(channel_done_flag == phy_adc_cfg.channel) {
        if(channel_done_flag & BIT(PHY_ADC_PIN_IDX)) {
            is_high_resolution = (phy_adc_cfg.is_high_resolution & BIT(PHY_ADC_PIN_IDX))?TRUE:FALSE;
            is_differential_mode = (phy_adc_cfg.is_differential_mode & BIT(PHY_ADC_PIN_IDX))?TRUE:FALSE;
            value = hal_adc_value_cal((adc_CH_t)PHY_ADC_PIN_IDX, adc_debug, pev->size, is_high_resolution, is_differential_mode);

            g_adc_value = (uint16_t)(value*1000);
        }

        channel_done_flag = 0;
    }
}

/*********************************************************
FN: 
*/
static void adcMeasureTask( void )
{
    int ret;
    uint8_t batt_ch = ADC_CH2N_P24;
    GPIO_Pin_e pin;

    pin = s_pinmap[batt_ch];
    hal_gpio_cfg_analog_io(pin, Bit_DISABLE);
    hal_gpio_write(pin, 1);  // lbl
    ret = hal_adc_config_channel(phy_adc_cfg, adc_evt);
    hal_gpio_cfg_analog_io(pin, Bit_DISABLE);

    if(ret) {
        LOG("ret = %d\n",ret);
        return;
    }

    hal_adc_start();
}

/*********************************************************
FN: 
*/
uint32_t ty_adc_init(ty_adc_t* p_adc)
{
    p_adc->channel = ADC_CH2N_P24;
    p_adc->value = 0;
    hal_adc_init();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_adc_start(ty_adc_t* p_adc)
{
    int i = 1000;
    ty_adc_init(p_adc);
    g_adc_value = 0;
    adcMeasureTask();
    
    while(i--) {
        ty_system_delay_ms(1);
        if(g_adc_value != 0) {
            p_adc->value = g_adc_value;
            ty_adc_stop(p_adc);
            return 0;
        }
    }

    ty_adc_stop(p_adc);
    
    return 1;
}

/*********************************************************
FN: 
*/
uint32_t ty_adc_stop(ty_adc_t* p_adc)
{
    hal_adc_stop();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_adc_control(ty_adc_t* p_adc, uint8_t cmd, void* arg)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_adc_uninit(ty_adc_t* p_adc)
{
    hal_adc_stop();
    return 0;
}








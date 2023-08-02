
#include "OSAL.h"
#include "log.h"
#include "gpio.h"
#include "led_driver.h"

#define LED_IDLE 0
#define LED_BUSY 1

#define LED_B_IO  P25
#define LED_R_IO  P24

#define LED_PRO_TIME_MS 10

static uint8_t led_task_id = 0;

static led_status_t led_status = 0;

typedef struct
{
    uint8_t status;
    uint16_t on_ms;     //亮时间  0xffff:常亮 0:常灭
    uint16_t off_ms;    //灭时间

}led_blink_t;

typedef struct
{
    uint8_t io;
    uint16_t cnt_ms;
    uint32_t status_mask;

    led_blink_t *blink;

}led_num_data_t;


typedef struct
{
    led_num_data_t ctr_data[LED_NUM_MAX];

}led_task_t;


static const led_blink_t blink_ctr[LED_MAX] = 
{
    {LED_INIT, 0, 0},
    {LED_KEY_RC_DATA_SEND, 30, 30},
    {LED_KEY_RC_NO_DATA_SEND, 150, 0xf000},
    {LED_KEY_RC_REC_DATA, 0, 0},
    {LED_KEY_RC_REC_DATA_DONE, 50, 50},
    {LED_KEY_RC_DEL_DATA, 50, 50},

    //ble sdk
    {LED_BLE_CONNECT, 0xffff, 0},

    //mcu sdk
    {LED_NET_CFG, 200, 200},
    {LED_NET_CONNECT, 500, 500},
    {LED_NET_CON_SUC, 1000, 1000},
    {LED_CLOUD_CON_SUC, 1000, 5000},
    {LED_WIFI_LOW_POWER, 0, 0}

};

static led_task_t led_data = {0};

void led_task_init(uint8_t task_id)
{
    led_task_id = task_id;

    hal_gpio_pin_init(LED_R_IO, GPIO_OUTPUT);
    hal_gpio_pull_set(LED_R_IO, GPIO_PULL_UP_S);
    hal_gpio_fast_write(LED_R_IO, 1);

    hal_gpio_pin_init(LED_B_IO, GPIO_OUTPUT);
    hal_gpio_pull_set(LED_B_IO, GPIO_PULL_UP_S);
    hal_gpio_fast_write(LED_B_IO, 1);

    //睡眠时保持IO状态
    hal_gpioretention_register(LED_R_IO);
    hal_gpioretention_register(LED_B_IO);

    for (uint8_t i = 0; i < LED_NUM_MAX; i++)
    {
        led_data.ctr_data[i].blink = (led_blink_t*)&blink_ctr[LED_INIT];
        led_data.ctr_data[i].status_mask = LED_INIT;
    }
    
    led_data.ctr_data[LED_BLUE].io = LED_B_IO;
    led_data.ctr_data[LED_RED].io = LED_R_IO;
}

uint16_t led_task_event(uint8_t task_id, uint16_t events)
{
    if (task_id != led_task_id)
    {
        return 0;
    }

    if (events & LED_TIME_PRO_EVENT)
    {

        if (led_pro(LED_PRO_TIME_MS))
        {
            osal_start_timerEx(led_task_id, LED_TIME_PRO_EVENT, LED_PRO_TIME_MS);
        }

        return (events ^ LED_TIME_PRO_EVENT);
    }

    return 0;
}

uint8_t led_pro(uint16_t tick_ms)
{
    led_num_data_t *p = NULL;
    uint8_t led_idle_cnt = 0;

    for (uint8_t i = 0; i < LED_NUM_MAX; i++)
    {
        p = &led_data.ctr_data[i];

        p->cnt_ms += tick_ms;

        if(p->blink->on_ms == 0)
        {
            hal_gpio_fast_write(p->io, 1);
        }
        else if(p->blink->on_ms == 0xffff)
        {
            hal_gpio_fast_write(p->io, 0);
        }
        else if(p->blink->on_ms > 0)
        {
            if((p->cnt_ms > (p->blink->on_ms + p->blink->off_ms)))
            {
                p->cnt_ms = 0;
            }
            else if(p->cnt_ms > p->blink->on_ms)
            {
                hal_gpio_fast_write(p->io, 1);
            }
            else if(p->cnt_ms <= p->blink->on_ms)
            {
                hal_gpio_fast_write(p->io, 0);
            }
        }

        if(p->status_mask == LED_INIT)
        {
            led_idle_cnt ++;
        }
    }

    if(led_idle_cnt == LED_NUM_MAX)
    {
        return LED_IDLE;
    }
    
    return LED_BUSY;
}

#define LED_STATUS_MASK_CHECK_SET(mask)                 \
    if(p->status_mask & (1 << mask))                    \
    {                                                   \
        if(p->blink->status != mask)                    \
        {                                               \
            p->blink = (led_blink_t*)&blink_ctr[mask];  \
            p->cnt_ms = 0;                              \
            return;                                     \
        }                                               \
    }                                                   \

static void led_red_status_mask(led_num_data_t *p)
{
    if(p->status_mask == 0)
    {
        p->blink = (led_blink_t*)&blink_ctr[LED_INIT];
        return;
    }

    //顺序表示优先级
    LED_STATUS_MASK_CHECK_SET(LED_KEY_RC_DATA_SEND);
    LED_STATUS_MASK_CHECK_SET(LED_KEY_RC_REC_DATA);
    LED_STATUS_MASK_CHECK_SET(LED_KEY_RC_REC_DATA_DONE);
    LED_STATUS_MASK_CHECK_SET(LED_KEY_RC_DEL_DATA);
    LED_STATUS_MASK_CHECK_SET(LED_KEY_RC_NO_DATA_SEND);
}

static void led_blue_status_mask(led_num_data_t *p)
{
    if(p->status_mask == 0)
    {
        p->blink = (led_blink_t*)&blink_ctr[LED_INIT];
        return;
    }

    //顺序表示优先级
    LED_STATUS_MASK_CHECK_SET(LED_NET_CFG);
    LED_STATUS_MASK_CHECK_SET(LED_NET_CONNECT);
    LED_STATUS_MASK_CHECK_SET(LED_NET_CON_SUC);
    LED_STATUS_MASK_CHECK_SET(LED_CLOUD_CON_SUC);
    LED_STATUS_MASK_CHECK_SET(LED_WIFI_LOW_POWER);
}

void led_set_status(led_num_t num, led_status_t status)
{
    led_num_data_t *p = NULL;

    if(num >= LED_NUM_MAX)
    {   
        return;
    }

    p = &led_data.ctr_data[num];

    if(p->status_mask & (1 << status) != 0)
    {
        return;
    }
    else
    {
        p->status_mask |= (1 << status);
    }

    switch(num)
    {
        case LED_BLUE:

            led_blue_status_mask(p);

            break;

        case LED_RED:

            led_red_status_mask(p);

            break;

        default:
            break;
    }

    osal_set_event(led_task_id, LED_TIME_PRO_EVENT);
    //osal_start_timerEx(led_task_id, LED_TIME_PRO_EVENT, LED_PRO_TIME_MS);
}

void led_clr_status(led_num_t num)
{
    led_num_data_t *p = NULL;

    if(num >= LED_NUM_MAX)
    {   
        return;
    }

    p = &led_data.ctr_data[num];

    p->status_mask = 0;
}

void led_reset_status(led_num_t num, led_status_t status)
{
    led_num_data_t *p = NULL;

    if(num >= LED_NUM_MAX)
    {   
        return;
    }

    p = &led_data.ctr_data[num];

    if(p->status_mask & (1 << status) == 0)
    {
        return;
    }
    else
    {
        p->status_mask &= (~(1 << status));
    }

    switch(num)
    {
        case LED_BLUE:

            led_blue_status_mask(p);

            break;

        case LED_RED:

            led_red_status_mask(p);

            break;

        default:
            break;
    }

    osal_set_event(led_task_id, LED_TIME_PRO_EVENT);
    //osal_start_timerEx(led_task_id, LED_TIME_PRO_EVENT, LED_PRO_TIME_MS);
}

led_status_t led_get_status(led_num_t num)
{
    return led_data.ctr_data[num].status_mask;
}

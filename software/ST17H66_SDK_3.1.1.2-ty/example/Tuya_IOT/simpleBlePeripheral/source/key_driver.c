
#include "OSAL.h"
#include "gpio.h"
#include "log.h"
#include "key_driver.h"

#define KEY_IDLE 0
#define KEY_BUSY 1

#define KEY_1_IO  P34
#define KEY_2_IO  P0
#define KEY_3_IO  P1

#define KEY_PRO_TIME_MS 10

#define KEY_UP_TIME_MS    20
#define KEY_DOWN_TIME_MS  50
#define KEY_LONG_TIME_MS   3000

#define KEY_DEBUG 1

#if KEY_DEBUG

#define KEY_LOG(...) LOG(__VA_ARGS__)

#else

#define KEY_LOG(...)

#endif

#define KEY_EVENT_CB(event)                                                     \
    if((key_scan.key_event & (1 << event)) == 0)                                  \
    {                                                                           \
        key_scan.key_event |= (1 << event);                                     \
        if(key_scan.cb != NULL)                                                 \
            key_scan.cb(key_scan.key_value, event);     \
    }                                                                             

typedef struct
{
    uint8_t key_value;

    uint8_t key_event;

    uint16_t key_down_cnt_ms;

    uint16_t key_up_cnt_ms;

    key_event_cb cb;

}key_scan_t;


static uint8_t key_task_id = 0;

static key_scan_t key_scan = {0};

static void key_int_cb(gpio_pin_e pin,gpio_polarity_e type)
{
    if((pin != KEY_1_IO) && (pin != KEY_2_IO) && (pin != KEY_3_IO))
    {
        return;
    }

    if(type == POL_FALLING)
    {
        osal_start_timerEx(key_task_id, KEY_TIME_PRO_EVENT, KEY_PRO_TIME_MS);
    }
}

static void key_driver_init(void)
{
    hal_gpio_pin_init(KEY_1_IO, GPIO_INPUT);
    hal_gpio_pull_set(KEY_1_IO, GPIO_PULL_UP_S);

    hal_gpioin_enable(KEY_1_IO);

    hal_gpio_pin_init(KEY_2_IO, GPIO_INPUT);
    hal_gpio_pull_set(KEY_2_IO, GPIO_PULL_UP_S);

    hal_gpioin_enable(KEY_2_IO);

    hal_gpio_pin_init(KEY_3_IO, GPIO_INPUT);
    hal_gpio_pull_set(KEY_3_IO, GPIO_PULL_UP_S);

    hal_gpioin_enable(KEY_3_IO);

    hal_gpioin_register(KEY_1_IO, NULL, key_int_cb);
    hal_gpioin_register(KEY_2_IO, NULL, key_int_cb);
    hal_gpioin_register(KEY_3_IO, NULL, key_int_cb);

}

static uint8_t key_read_status(void)
{
    uint8_t ret = 0;
    uint8_t key_status = 0;

    key_status = ((hal_gpio_read(KEY_1_IO) << KEY_1) | (hal_gpio_read(KEY_2_IO) << KEY_2) | (hal_gpio_read(KEY_3_IO) << KEY_3)) ^ 0x0e;

    //KEY_LOG("key_status = %02x\n", key_status);

    //每次只允许一种按键触发，组合键优先判断
    if (key_status == ((1 << KEY_3) | (1 << KEY_2)))
    {
        ret = KEY_2_3;
    }
    else if (key_status == ((1 << KEY_1) | (1 << KEY_2)))
    {
        ret = KEY_1_2;
    }
    else if(key_status == (1 << KEY_1))
    {
        ret = KEY_1;
    }
    else if(key_status == (1 << KEY_2))
    {
        ret = KEY_2;
    }
    else if(key_status == (1 << KEY_3))
    {
        ret = KEY_3;
    }
    
    return ret;
}

void key_task_init(uint8_t task_id)
{
    key_task_id = task_id;

    key_driver_init();

    key_scan.cb = NULL;
}

static uint8_t key_pro(uint16_t tick_ms)
{
    uint8_t ret = KEY_IDLE;
    uint8_t key_value = key_read_status();

    if(key_value != 0)
    {
        ret = KEY_BUSY;

        if(key_scan.key_value != key_value)
        {
            if((key_scan.key_value != 0) && (key_scan.key_event != 0))
            {
                KEY_EVENT_CB(KEY_EVENT_UP);
            }

            key_scan.key_down_cnt_ms = 0;
            key_scan.key_up_cnt_ms = 0;
            key_scan.key_event = 0;

            key_scan.key_value = key_value;
        }
        else
        {
            key_scan.key_down_cnt_ms += tick_ms;

            if(key_scan.key_down_cnt_ms >= KEY_LONG_TIME_MS)
            {
                KEY_EVENT_CB(KEY_EVENT_LONG);
            }
            else if (key_scan.key_down_cnt_ms >= KEY_DOWN_TIME_MS)
            {
                KEY_EVENT_CB(KEY_EVENT_DOWN);
            }
        }
    }
    else
    {
        if(key_scan.key_event != 0)
        {
            key_scan.key_up_cnt_ms += tick_ms;

            if(key_scan.key_up_cnt_ms >= KEY_UP_TIME_MS)
            {
                KEY_EVENT_CB(KEY_EVENT_UP);

                key_scan.key_down_cnt_ms = 0;
                key_scan.key_up_cnt_ms = 0;
                key_scan.key_event = 0;

                key_scan.key_value = 0;

                ret = KEY_IDLE;
            }
            else
            {
                ret = KEY_BUSY;
            }
        }
    }

    return ret;
}

uint16_t key_task_event(uint8_t task_id, uint16_t events)
{
    if (task_id != key_task_id)
    {
        return 0;
    }

    if (events & KEY_TIME_PRO_EVENT)
    {

        if(key_pro(KEY_PRO_TIME_MS) == KEY_BUSY)
        {
            osal_start_timerEx(key_task_id, KEY_TIME_PRO_EVENT, KEY_PRO_TIME_MS);
        }

        return (events ^ KEY_TIME_PRO_EVENT);
    }

    return 0;
}

void key_set_event_cb(key_event_cb cb)
{
    key_scan.cb = cb;
}


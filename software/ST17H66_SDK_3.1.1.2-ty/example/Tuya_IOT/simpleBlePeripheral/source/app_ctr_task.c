
#include "OSAL.h"
#include "log.h"
#include "app_ctr_task.h"
#include "key_driver.h"
#include "rc_task.h"
#include "led_driver.h"
#include "osal_snv.h"
#include "ty_dev_config.h"

#if(TY_BLE_SDK == 1)

#include "tuya_ble_api.h"


#else

#include "ty_mcu_sdk_app.h"
#include "wifi.h"

#endif


#define RC_DATA_FS_ID   0xe2
#define APP_CTR_PRO_TIME_MS 10


#define APP_CTR_DEBUG 1

#if APP_CTR_DEBUG

#define APP_CTR_LOG(...) LOG(__VA_ARGS__)

#else

#define APP_CTR_LOG(...)

#endif

typedef struct
{
    uint8_t available;

    rc_send_data_t data;

}app_ctr_rc_data_t;


typedef struct
{
    app_ctr_rc_data_t fs_data[3];

}app_fs_rc_data_t;


typedef struct
{
    uint8_t rc_send_index;

    uint8_t rc_rec_index;

    uint8_t rc_send_once;

    app_fs_rc_data_t rc_data;

}app_ctr_data_t;



static uint8_t app_ctr_task_id = 0;

app_ctr_data_t app_ctr_data = {0};

static void app_ctr_rc_data_send(uint8_t index)
{
    if (app_ctr_data.rc_data.fs_data[index - 1].available == 1)
    {
        app_ctr_data.rc_send_index = index;
        osal_set_event(app_ctr_task_id, APP_SEND_EVENT);
    }
    else
    {
        led_set_status(LED_RED, LED_KEY_RC_NO_DATA_SEND);
    }
}

static void app_ctr_rc_data_rec(uint8_t index)
{
    if (app_ctr_data.rc_data.fs_data[index - 1].available == 0)
    {
        rc_rec_start();

        app_ctr_data.rc_rec_index = index;
    }
}

static void app_task_key_event_cb(uint8_t key_value, uint8_t event)
{
    APP_CTR_LOG("key_value = %d, event = %d\n", key_value, event);

    switch (event)
    {
    case KEY_EVENT_DOWN:
        APP_CTR_LOG("rc_data.fs_data.available =  %d %d %d \n", app_ctr_data.rc_data.fs_data[0].available, app_ctr_data.rc_data.fs_data[1].available, app_ctr_data.rc_data.fs_data[2].available);
        switch (key_value)
        {
        case KEY_1:

            app_ctr_rc_data_send(1);

            break;

        case KEY_2:

            app_ctr_rc_data_send(2);

            // if (app_ctr_data.rc_data.fs_data[1].available == 1)
            // {
            //     app_ctr_data.rc_send_index = 2;
            //     osal_set_event(app_ctr_task_id, APP_SEND_EVENT);
            // }
            // else
            // {
            //     led_set_status(LED_RED, LED_KEY_RC_NO_DATA_SEND);
            // }

            break;

        case KEY_3:

            app_ctr_rc_data_send(3);

            break;

        default:
            break;
        }

        break;

    case KEY_EVENT_UP:

        app_ctr_data.rc_send_index = 0;
        rc_rec_stop();

        if(app_ctr_data.rc_rec_index != 0)
        {
            app_ctr_data.rc_rec_index = 0;
            led_reset_status(LED_RED, LED_KEY_RC_REC_DATA_DONE);
        }

        led_reset_status(LED_RED, LED_KEY_RC_DEL_DATA);
        led_reset_status(LED_RED, LED_KEY_RC_NO_DATA_SEND);
        led_reset_status(LED_RED, LED_KEY_RC_DATA_SEND);

        break;

    case KEY_EVENT_LONG:

        switch (key_value)
        {
        case KEY_1:

            app_ctr_rc_data_rec(1);

            break;

        case KEY_2:

            app_ctr_rc_data_rec(2);

            break;

        case KEY_3:

            app_ctr_rc_data_rec(3);

            // if (app_ctr_data.rc_data.fs_data[2].available == 0)
            // {
            //     rc_rec_start();

            //     app_ctr_data.rc_rec_index = 3;
            // }

            break;

        case KEY_1_2:

            osal_memset(&app_ctr_data.rc_data, 0, sizeof(app_fs_rc_data_t));

            osal_snv_write(RC_DATA_FS_ID, sizeof(app_fs_rc_data_t), (uint8_t*)&app_ctr_data.rc_data);

            led_set_status(LED_RED, LED_KEY_RC_DEL_DATA);

            break;

        case KEY_2_3:

            //清除配置/添加配置
            #if(TY_BLE_SDK == 1)

            //绑定状态就接触绑定
            if(tuya_ble_connect_status_get() != UNBONDING_UNCONN)
            {
                tuya_ble_device_factory_reset();
            }

            #else

             LOG("TY_KEY_LONG_EVENT, state = %d\r\n", mcu_get_wifi_work_state());
            //判断当前状态，未配网->进入配网，已配网->退网
            switch(mcu_get_wifi_work_state())
            {
                case SMART_CONFIG_STATE:
                case AP_STATE:
                case SMART_AND_AP_STATE:

                    //配网中，不处理

                    break;
                case WIFI_NOT_CONNECTED:
                case WIFI_CONNECTED:
                case WIFI_CONN_CLOUD:

                    //退网
                    mcu_reset_wifi();

                    break;

                case WIFI_LOW_POWER:

                    //进入配网
                    mcu_set_wifi_mode(AP_CONFIG);

                    break;

                default:
                    break;  
            }

            #endif

            break;

        default:
            break;
        }

        if(app_ctr_data.rc_rec_index != 0)
        {
            led_set_status(LED_RED, LED_KEY_RC_REC_DATA);
        }

        break;
    
    default:
        break;
    }
}

static void app_task_rc_event_cb(uint8_t event, void *args)
{
    APP_CTR_LOG("rc event = %d\n", event);

    switch(event)
    {
        case RC_RECEIVE_AVAILABLE:

            if(args == NULL)
            {
                break;
            }

            rc_send_data_t *data = (rc_send_data_t *)args;
        
            osal_memcpy(&app_ctr_data.rc_data.fs_data[app_ctr_data.rc_rec_index - 1].data, data, sizeof(rc_send_data_t));
            app_ctr_data.rc_data.fs_data[app_ctr_data.rc_rec_index - 1].available = 1;

            APP_CTR_LOG("rc rec = %d %d  %x  %d %d\n", app_ctr_data.rc_rec_index - 1, data->send_protocol, data->send_value, data->send_bit_len, app_ctr_data.rc_data.fs_data[app_ctr_data.rc_rec_index - 1].available);

            osal_snv_write(RC_DATA_FS_ID, sizeof(app_fs_rc_data_t), (uint8_t*)&app_ctr_data.rc_data);

            led_reset_status(LED_RED, LED_KEY_RC_REC_DATA);
            led_set_status(LED_RED, LED_KEY_RC_REC_DATA_DONE);

            break;
    }
}

void app_ctr_task_init(uint8_t task_id)
{
    app_ctr_task_id = task_id;

    key_set_event_cb(app_task_key_event_cb);

    rc_set_event_cb(app_task_rc_event_cb);

    if(osal_snv_read(RC_DATA_FS_ID, sizeof(app_fs_rc_data_t), (uint8_t*)&app_ctr_data.rc_data) == PPlus_SUCCESS)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            APP_CTR_LOG("fs_data[%d], available = %d, protocol = %d, value = 0x%x, pluse_len = %d, bit_len = %d \n", 
            i, app_ctr_data.rc_data.fs_data[i].available, app_ctr_data.rc_data.fs_data[i].data.send_protocol, 
            app_ctr_data.rc_data.fs_data[i].data.send_value, app_ctr_data.rc_data.fs_data[i].data.send_pulse_len, app_ctr_data.rc_data.fs_data[i].data.send_bit_len);
        }
        
    }
}

uint16_t app_ctr_task_event(uint8_t task_id, uint16_t events)
{
    if (task_id != app_ctr_task_id)
    {
        return 0;
    }

    if (events & APP_TIME_PRO_EVENT)
    {

        return (events ^ APP_TIME_PRO_EVENT);
    }

    if (events & APP_SEND_ONCE_EVENT)
    {

        app_ctr_data.rc_send_once = 0;
        app_ctr_data.rc_send_index = 0;

        led_reset_status(LED_RED, LED_KEY_RC_NO_DATA_SEND);
        led_reset_status(LED_RED, LED_KEY_RC_DATA_SEND);

        return (events ^ APP_SEND_ONCE_EVENT);
    }

    if (events & APP_SEND_EVENT)
    {
        if(app_ctr_data.rc_send_index)
        {
            APP_CTR_LOG("rc send = %d\n", app_ctr_data.rc_send_index);

            led_set_status(LED_RED, LED_KEY_RC_DATA_SEND);

            rc_send(&app_ctr_data.rc_data.fs_data[app_ctr_data.rc_send_index - 1].data);
            osal_start_timerEx(app_ctr_task_id, APP_SEND_EVENT, 300);
        }

        return (events ^ APP_SEND_EVENT);
    }

    return 0;
}

void app_ctr_rc_send_once(uint8_t index)
{
    if(app_ctr_data.rc_send_index != 0)
    {
        return;
    }

    app_ctr_data.rc_send_once = 1;

    app_ctr_rc_data_send(index);

    osal_start_timerEx(app_ctr_task_id, APP_SEND_ONCE_EVENT, 610);
}

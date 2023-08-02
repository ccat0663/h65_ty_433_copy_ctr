
#include "OSAL.h"
#include "log.h"
#include "rc_task.h"

typedef struct
{
    uint8_t rec_start;

    rc_event_cb cb;

}rc_task_t;

#define RC_PRO_TIME_MS 10

#define RC_TASK_DEBUG 1

#if RC_TASK_DEBUG

#define RC_TASK_LOG(...) LOG(__VA_ARGS__)

#else

#define RC_TASK_LOG(...)

#endif

static uint8_t rc_task_id = 0;

static rc_task_t rc_task = {0};

void rc_task_init(uint8_t task_id)
{
    rc_task_id = task_id;

    initSwich();
}

uint16_t rc_task_event(uint8_t task_id, uint16_t events)
{
    if (task_id != rc_task_id)
    {
        return 0;
    }

    if (events & RC_TIME_PRO_EVENT)
    {
        if(available() == TRUE)
        {

            RC_TASK_LOG("pro = %d  %x  %d %d\n", getReceivedProtocol(), getReceivedValue(), getReceivedBitlength(), getReceivedDelay());

            if(rc_task.cb)
            {
                rc_send_data_t data = {0};

                data.send_protocol = getReceivedProtocol();
                data.send_bit_len = getReceivedBitlength();
                data.send_value = getReceivedValue();
                data.send_pulse_len = getReceivedDelay();

                rc_task.cb(RC_RECEIVE_AVAILABLE, (void *)&data);
            }

            resetAvailable();

            rc_rec_stop();
        }

        if(rc_task.rec_start ==  1)
        {
            osal_start_timerEx(rc_task_id, RC_TIME_PRO_EVENT, RC_PRO_TIME_MS);
        }

        return (events ^ RC_TIME_PRO_EVENT);
    }

    return 0;
}

uint8_t rc_send(rc_send_data_t *data)
{
    if((rc_task.rec_start == 1) || (data == NULL))
    {
        return 0;
    }

    RC_TASK_LOG("rc_send = %d  %x  %d %d\n", data->send_protocol, data->send_value, data->send_bit_len, data->send_pulse_len);

    // enableTransmit(0);
    //setProtocol(data->send_protocol);
    setProtocolPulseLength(data->send_protocol, data->send_pulse_len);
    // setRepeatTransmit(5);
    // send(data->send_value, data->send_bit_len);
    // disableTransmit();
    return send_nonsync(data->send_value, data->send_bit_len, 5);
}

uint8_t rc_rec_start(void)
{
    RC_TASK_LOG("rc_task rec_start\n");

    rc_task.rec_start = 1;

    enableReceive(0);

    osal_start_timerEx(rc_task_id, RC_TIME_PRO_EVENT, RC_PRO_TIME_MS);
}

uint8_t rc_rec_stop(void)
{
    RC_TASK_LOG("rc_task rec_stop\n");

    rc_task.rec_start = 0;

    disableReceive();
}

void rc_set_event_cb(rc_event_cb cb)
{
    rc_task.cb = cb;
}


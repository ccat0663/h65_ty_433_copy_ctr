#include "uart.h"
#include "ty_uart.h"
#include "tuya_ble_api.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define TY_UART_TX_BUF_SIZE       256

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static uint8_t s_uart_tx_buf[TY_UART_TX_BUF_SIZE] = {0};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
__ATTR_SECTION_SRAM__ void phy_uart_evt_hdl(uart_Evt_t* pev)
{
    switch(pev->type)
    {
        case UART_EVT_TYPE_RX_DATA:
        case  UART_EVT_TYPE_RX_DATA_TO: {
            tuya_ble_common_uart_receive_data(pev->data, pev->len);
        } break;

        case UART_EVT_TYPE_TX_COMPLETED: {
        } break;

        default: {
        } break;
    }
}

/*********************************************************
FN: 
*/
uint32_t ty_uart_init(void)
{
    //Log
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_uart2_init(void)
{
    uart_Cfg_t cfg = {
        .tx_pin = P25,
        .rx_pin = P26,
        .rts_pin = GPIO_DUMMY,
        .cts_pin = GPIO_DUMMY,
        .baudrate = 9600,
        .use_fifo = TRUE,
        .hw_fwctrl = FALSE,
        .use_tx_buf = TRUE,
        .parity     = FALSE,
        .evt_handler = phy_uart_evt_hdl,
    };
    
    hal_uart_deinit(UART1);
    hal_uart_init(cfg, UART1);
    hal_uart_set_tx_buf(UART1, s_uart_tx_buf, TY_UART_TX_BUF_SIZE);
    
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_uart_send(const uint8_t* buf, uint32_t size)
{
    //Log
  	return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_uart2_send(const uint8_t* buf, uint32_t size)
{
    hal_uart_send_buff(UART1, (uint8_t*)buf, size);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_uart_uninit(void)
{
    //Log
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_uart2_uninit(void)
{
    hal_uart_deinit(UART1);
    return 0;
}







/**
****************************************************************************
* @file      ty_uart.h
* @brief     ty_uart
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_UART_H__
#define __TY_UART_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "board.h"

/*********************************************************************
 * CONSTANT
 */

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_uart_init(void);
uint32_t ty_uart2_init(void);
uint32_t ty_uart_send(const uint8_t* buf, uint32_t size);
uint32_t ty_uart2_send(const uint8_t* buf, uint32_t size);
uint32_t ty_uart_uninit(void);
uint32_t ty_uart2_uninit(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_UART_H__

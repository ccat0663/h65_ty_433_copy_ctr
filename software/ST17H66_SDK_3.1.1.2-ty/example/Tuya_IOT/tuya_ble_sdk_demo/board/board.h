/**
****************************************************************************
* @file      ty_template.h
* @brief     ty_template
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_TEMPLATE_H__
#define __TY_TEMPLATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDE
 */
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "tuya_ble_type.h"
#include "elog.h"

/*********************************************************************
 * CONSTANT
 */
#define TY_LOG_ENABLE                           1//TUYA_APP_LOG_ENABLE	//0 //
#if (TY_LOG_ENABLE)
    #define TY_PRINTF(...)                      log_d(__VA_ARGS__)
    #define TY_HEXDUMP_0(...)                   elog_hexdump("", 8, __VA_ARGS__)
    #define TY_HEXDUMP(_fmt_, p_data, len)      \
                                                do {                                            \
                                                    TY_PRINTF( _fmt_ " [len=%d] :", len);       \
                                                    TY_HEXDUMP_0(p_data, len);                  \
                                                } while(0)
#else
    #define TY_PRINTF(...)
    #define TY_HEXDUMP(...)
#endif

#define CHIP_PLATFORM_NRF52832                  0

#define BOARD_FLASH_BULK_DATA_START_ADDR        (0x4C000)
#define BOARD_FLASH_TUYA_INFO_START_ADDR        (0x50000)
#define BOARD_FLASH_OTA_INFO_ADDR               (0x54000)
#define BOARD_FLASH_OTA_START_ADDR              (0x55000)
#define BOARD_FLASH_OTA_END_ADDR                (0x80000)
//#define BOARD_FLASH_MAC_START_ADDR              0

#define BOARD_PWM_PIN                           14
#define BOARD_ADC_CHANNEL                       24

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */


#ifdef __cplusplus
}
#endif

#endif //__TY_TEMPLATE_H__

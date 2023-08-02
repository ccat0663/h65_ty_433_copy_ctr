/**
****************************************************************************
* @file      ty_rtc.h
* @brief     ty_rtc
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_RTC_H__
#define __TY_RTC_H__

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
uint32_t ty_rtc_init(void);
uint32_t ty_rtc_set_time(uint32_t timestamp);
uint32_t ty_rtc_get_time(uint32_t* p_timestamp);
uint32_t ty_rtc_get_local_time(uint32_t* p_timestamp);
uint32_t ty_rtc_get_old_time(uint32_t old_local_timestamp, uint32_t* p_timestamp);
uint32_t ty_rtc_control(uint8_t cmd, void* arg);
uint32_t ty_rtc_uninit(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_RTC_H__

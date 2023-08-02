/**
****************************************************************************
* @file      ty_system.h
* @brief     ty_system
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_SYSTEM_H__
#define __TY_SYSTEM_H__

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
uint32_t ty_system_init(uint8_t location);
uint32_t ty_system_mainloop(void);
uint32_t ty_system_reset(void);
uint32_t ty_system_wdt_init(void);
uint32_t ty_system_wdt_uninit(void);
uint32_t ty_system_wdt_feed(void);
uint32_t ty_system_log_init(void);
uint32_t ty_system_log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size);
uint32_t ty_system_delay_ms(uint32_t ms);
uint32_t ty_system_delay_us(uint32_t us);
uint32_t ty_system_enter_sleep(void);
uint32_t ty_system_exit_sleep(void);
bool ty_system_is_sleep(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_SYSTEM_H__

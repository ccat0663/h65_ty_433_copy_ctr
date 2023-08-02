/**
****************************************************************************
* @file      ty_timer.h
* @brief     ty_timer
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_TIMER_H__
#define __TY_TIMER_H__

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
typedef enum {
    TY_TIMER_SINGLE_SHOT,
    TY_TIMER_REPEATED,
} ty_timer_mode_t;

/*********************************************************************
 * STRUCT
 */
typedef void (*ty_timer_handler_t)(void*);

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
uint32_t ty_timer_create(void** p_timer_id, uint32_t ms, ty_timer_mode_t mode, ty_timer_handler_t handler);
uint32_t ty_timer_delete(void* timer_id);
uint32_t ty_timer_start(void* timer_id);
uint32_t ty_timer_stop(void* timer_id);
uint32_t ty_timer_restart(void* timer_id, uint32_t ms);


#ifdef __cplusplus
}
#endif

#endif //__TY_TIMER_H__

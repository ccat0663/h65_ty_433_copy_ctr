/**
****************************************************************************
* @file      ty_key_press.h
* @brief     ty_key_press
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_KEY_PRESS_H__
#define __TY_KEY_PRESS_H__

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
    TY_KEY_PRESS_LEVEL_LOW  = 0,
    TY_KEY_PRESS_LEVEL_HIGH = 1,
} ty_key_press_level_t;

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
/*
state=0 - idle
state=1 - press1
state=2 - press2
state=3 - release, 超时释放
state=4 - release0, count0之前释放
state=5 - release1
state=6 - release2

详见README
*/
typedef void (*ty_key_press_handler_t)(uint32_t state);

typedef struct {
    uint32_t count;
    uint32_t check_idx;
    uint32_t check_record;
    uint32_t state;
} ty_key_press_inter_param_t;

typedef struct {
    uint32_t pin;
    ty_key_press_level_t valid_level;
    uint8_t  count_len;
    uint32_t count_array[10];
    ty_key_press_handler_t handler;
    ty_key_press_inter_param_t inter; //内部变量，不用赋值
} ty_key_press_param_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
//__weak, to redefine by user
uint32_t ty_key_press_get_pin_level(uint32_t pin);

uint32_t ty_key_press_init(ty_key_press_param_t* param);
void ty_key_press_timeout_handler(ty_key_press_param_t* param);
void ty_key_press_test(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_KEY_PRESS_H__

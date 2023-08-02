#include "ty_key_press.h"
#include "ty_util.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define KEY_CHECK_TOTAL        5
#define KEY_CHECK_RELEASE      3

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
__TUYA_BLE_WEAK uint32_t ty_key_press_get_pin_level(uint32_t pin)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_key_press_init(ty_key_press_param_t* param)
{
    //param check
    for(uint32_t idx=0; idx<param->count_len; idx++) {
        if(param->count_array[idx] == 0) {
            return 1;
        }
    }
    return 0;
}

/*********************************************************
FN: 按键超时处理
*/
void ty_key_press_timeout_handler(ty_key_press_param_t* param)
{
    bool key_pressed = true;
    
    param->inter.count++;
    
    param->inter.check_idx++;
    if(param->inter.check_idx == KEY_CHECK_TOTAL) {
        param->inter.check_idx = 0;
    }
    
    //记录前N次的状态
    if(ty_key_press_get_pin_level(param->pin) == param->valid_level) {
        param->inter.check_record &= ~(1<<param->inter.check_idx);
    } else {
        param->inter.check_record |= (1<<param->inter.check_idx);
    }
    //无效次数大于n次
    if(ty_util_count_one_in_num(param->inter.check_record) >= KEY_CHECK_RELEASE) {
        key_pressed = false;
    }
    
    if(key_pressed == true) {
        for(uint32_t idx=0; idx<param->count_len; idx++) {
            if(param->inter.count == param->count_array[idx]) {
                param->inter.state = idx+1;
                
                if(idx < (param->count_len-1)) {
                    param->handler(param->inter.state);
                }
                break;
            }
        }
    } else {
        for(uint32_t idx=0; idx<param->count_len; idx++) {
            if(idx == 0) {
                if(param->inter.count <= param->count_array[idx]) {
                    param->inter.state = idx+1+param->count_len;
                    break;
                }
            } else {
                if((param->inter.count > param->count_array[idx-1]) \
                    && (param->inter.count <= param->count_array[idx])) {
                    param->inter.state = idx+1+param->count_len;
                    break;
                }
            }
        }
    }
    
    //释放
    if(param->inter.state >= param->count_len) {
        param->handler(param->inter.state);
        
        param->inter.count = 0;
        param->inter.check_idx = 0;
        param->inter.check_record = 0;
        param->inter.state = 0;
    }
}




/*********************************************************  Demo  *********************************************************/

/*********************************************************************
 * LOCAL FUNCTION
 */
static void suble_key_state_handler(uint32_t state);

/*********************************************************************
 * VARIABLE
 */
ty_key_press_param_t param = {
    .pin = 0,
    .valid_level = TY_KEY_PRESS_LEVEL_LOW,
    .count_len = 3,
    .count_array = {5, 300, 500},
    .handler = suble_key_state_handler,
};

/*********************************************************
FN: 按键处理函数
*/
static void suble_key_state_handler(uint32_t state)
{
    switch(state)
    {
        case 1: { //单击，按下
        } break;
        
        case 2: { //长按3s，按下
        } break;
        
        case 3: { //长按5s，按下
        } break;
        
        case 5: { //单击，释放
        } break;
        
        case 6: { //长按，3~5s内释放
        } break;
        
        default: {
        } break;
    }
    
    if(state >= param.count_len) {
        //关闭定时器
    }
}

/*********************************************************
FN: 
*/
void ty_key_press_test(void)
{
    ty_key_press_init(&param);
}








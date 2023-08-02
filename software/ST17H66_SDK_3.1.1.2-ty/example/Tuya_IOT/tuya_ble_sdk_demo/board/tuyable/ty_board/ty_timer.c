#include "ty_timer.h"
#include "bcomdef.h"
#include "osal_Timers.h"
#include "simpleBLEPeripheral.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define TY_TIMER_MAX_NUM                    16

#define TY_TIMER_HANDLER(x, y) \
            if(timer_id & PHY_TIMER##x##_EVT) { \
                ty_timer_pool[y].handler(&timer_id); \
                return (timer_id^PHY_TIMER##x##_EVT); \
            }
            
/*********************************************************************
 * LOCAL STRUCT
 */
//#pragma pack(1)
typedef struct {
    uint8_t  is_occupy;
    uint16_t timer_id;
    uint32_t mode;
    uint32_t ms;
    ty_timer_handler_t handler;
} ty_timer_item_t;
//#pragma pack()

/*********************************************************************
 * LOCAL VARIABLE
 */
static ty_timer_item_t ty_timer_pool[TY_TIMER_MAX_NUM] = {
    [0]  = { .timer_id = PHY_TIMER0_EVT},
    [1]  = { .timer_id = PHY_TIMER1_EVT},
    [2]  = { .timer_id = PHY_TIMER2_EVT},
    [3]  = { .timer_id = PHY_TIMER3_EVT},
    [4]  = { .timer_id = PHY_TIMER4_EVT},
    [5]  = { .timer_id = PHY_TIMER5_EVT},
    [6]  = { .timer_id = PHY_TIMER6_EVT},
    [7]  = { .timer_id = PHY_TIMER7_EVT},
    [8]  = { .timer_id = PHY_TIMER8_EVT},
    [9]  = { .timer_id = PHY_TIMER9_EVT},
    [10]  = { .timer_id = PHY_TIMER10_EVT},
    [11]  = { .timer_id = PHY_TIMER11_EVT},
    [12]  = { .timer_id = PHY_TIMER12_EVT},
    [13]  = { .timer_id = PHY_TIMER13_EVT},
    [14]  = { .timer_id = PHY_TIMER14_EVT},
    [15]  = { .timer_id = PHY_TIMER15_EVT},
};

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
static ty_timer_item_t* acquire_timer(uint32_t ms, ty_timer_mode_t mode, ty_timer_handler_t handler)
{
    for(uint8_t i=0; i<TY_TIMER_MAX_NUM; i++) {
        if (ty_timer_pool[i].is_occupy == 0) {
            ty_timer_pool[i].is_occupy = 1;
            ty_timer_pool[i].mode = mode;
            ty_timer_pool[i].ms = ms;
            ty_timer_pool[i].handler = handler;
            return (void*)&ty_timer_pool[i];
        }
    }
}

/*********************************************************
FN: 
*/
static int32_t release_timer(void* timer_id)
{
    for(uint8_t i=0; i<TY_TIMER_MAX_NUM; i++) {
        if (timer_id == &ty_timer_pool[i]) {
            ty_timer_pool[i].is_occupy = 0;
            return i;
        }
    }
    return -1;
}

/*********************************************************
FN: 
*/
static int32_t find_timer_ms(void* timer_id, uint32_t *ms)
{
    for(uint8_t i=0; i<TY_TIMER_MAX_NUM; i++) {
        if (timer_id == &ty_timer_pool[i]) {
            *ms = ty_timer_pool[i].ms;
            return i;
        }
    }
    return -1;
}

/*********************************************************
FN: 
*/
uint16 phy_timer_handler(uint16_t timer_id)
{
    TY_TIMER_HANDLER(0, 0);
    TY_TIMER_HANDLER(1, 1);
    TY_TIMER_HANDLER(2, 2);
    TY_TIMER_HANDLER(3, 3);
    TY_TIMER_HANDLER(4, 4);
    TY_TIMER_HANDLER(5, 5);
    TY_TIMER_HANDLER(6, 6);
    TY_TIMER_HANDLER(7, 7);
    TY_TIMER_HANDLER(8, 8);
    TY_TIMER_HANDLER(9, 9);
    TY_TIMER_HANDLER(10, 10);
    TY_TIMER_HANDLER(11, 11);
    TY_TIMER_HANDLER(12, 12);
    TY_TIMER_HANDLER(13, 13);
    TY_TIMER_HANDLER(14, 14);
    TY_TIMER_HANDLER(15, 15);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_timer_create(void** p_timer_id, uint32_t ms, ty_timer_mode_t mode, ty_timer_handler_t handler)
{
    ty_timer_item_t* timer_item = acquire_timer(ms, mode, handler);
    if (timer_item == NULL) {
        return 1;
    }
    *p_timer_id = timer_item;
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_timer_delete(void* timer_id)
{
    ty_timer_item_t* timer_item = timer_id;
    int id = release_timer(timer_item);
    if (id == -1) {
        return 1;
    }
    
    osal_stop_timerEx(phy_timer_TaskID, timer_item->timer_id);
}

/*********************************************************
FN: 
*/
uint32_t ty_timer_start(void* timer_id)
{
    uint32_t find_ms;
    ty_timer_item_t* timer_item = timer_id;
    
    if(find_timer_ms(timer_id, &find_ms) >= 0) {
        extern void osalTimeUpdate(void);
        osalTimeUpdate();
        if(timer_item->mode == TY_TIMER_SINGLE_SHOT) {
            osal_start_timerEx(phy_timer_TaskID, timer_item->timer_id, find_ms);
        } else if(timer_item->mode == TY_TIMER_REPEATED) {
            osal_start_reload_timer(phy_timer_TaskID, timer_item->timer_id, find_ms);
        }
        return 0;
    } else {
        TY_PRINTF("Error: Timer id is not found");
        return 1;
    }
}

/*********************************************************
FN: 
*/
uint32_t ty_timer_stop(void* timer_id)
{
    ty_timer_item_t* timer_item = timer_id;
    osal_stop_timerEx(phy_timer_TaskID, timer_item->timer_id);
}

/*********************************************************
FN: 
*/
uint32_t ty_timer_restart(void* timer_id, uint32_t ms)
{
}










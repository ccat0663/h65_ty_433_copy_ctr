#include "ty_flash.h"
#include "ty_system.h"
#include "flash.h"
#include "tuya_ble_config.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static ALIGN4_U8 ty_flash_cache[256];

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
uint32_t ty_flash_init(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_flash_read(uint32_t addr, uint8_t* buf, uint32_t size)
{
    ty_system_wdt_feed();
    
    return hal_flash_read(addr,buf,size);
}

/*********************************************************
FN: 
*/
uint32_t ty_flash_write(uint32_t addr, const uint8_t* buf, uint32_t size)
{
    ty_system_wdt_feed();
    
    memcpy(ty_flash_cache, buf, size);
    return hal_flash_write_by_dma(addr, ty_flash_cache, size);
//    return hal_flash_write(addr, ty_flash_cache, size);
}

/*********************************************************
FN: 
*/
uint32_t ty_flash_erase(uint32_t addr, uint32_t num)
{
    ty_system_wdt_feed();
    
    if(addr % TUYA_NV_ERASE_MIN_SIZE != 0) {
        TY_PRINTF("the start address is a not multiple of TUYA_NV_ERASE_MIN_SIZE");
        return TUYA_BLE_ERR_INVALID_ADDR;
    }

    for(uint32_t idx = 0; idx < num; idx++) {
        hal_flash_erase_sector(addr + (TUYA_NV_ERASE_MIN_SIZE*idx));
    }
    
    return TUYA_BLE_SUCCESS;
}







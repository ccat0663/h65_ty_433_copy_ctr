/**
****************************************************************************
* @file      ty_flash.h
* @brief     ty_flash
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_FLASH_H__
#define __TY_FLASH_H__

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
uint32_t ty_flash_init(void);
uint32_t ty_flash_read(uint32_t addr, uint8_t* buf, uint32_t size);
uint32_t ty_flash_write(uint32_t addr, const uint8_t* buf, uint32_t size);
uint32_t ty_flash_erase(uint32_t addr, uint32_t num);


#ifdef __cplusplus
}
#endif

#endif //__TY_FLASH_H__

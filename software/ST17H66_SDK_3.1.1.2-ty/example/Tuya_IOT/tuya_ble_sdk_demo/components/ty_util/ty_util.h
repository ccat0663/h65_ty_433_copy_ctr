/**
****************************************************************************
* @file      ty_util.h
* @brief     ty_util
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_UTIL_H__
#define __TY_UTIL_H__

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
uint8_t  ty_util_check_sum8(uint8_t* buf, uint32_t size);
uint16_t ty_util_check_sum16(uint8_t* buf, uint32_t size);
uint16_t ty_util_crc16(uint8_t* buf, uint32_t size, uint16_t* p_crc);
uint32_t ty_util_crc32(uint8_t* buf, uint32_t size, uint32_t* p_crc);
uint32_t ty_util_intArray2int(uint8_t* intArray, uint32_t startIdx, uint32_t size);
uint32_t ty_util_int2intArray(uint32_t num, uint8_t* intArray, uint32_t size);
uint32_t ty_util_reverse_byte(void* buf, uint32_t size);
uint32_t ty_util_count_one_in_num(uint32_t num);
uint32_t ty_util_shell_sort(int* buf, int size);

uint8_t  ty_util_str_hexChar2int(uint8_t hexChar);
uint8_t  ty_util_str_int2hexChar(bool isHEX, uint8_t intNum);
uint32_t ty_util_str_hexStr2int(uint8_t* hexStr, uint32_t size, uint32_t* num);
uint32_t ty_util_str_int2hexStr(bool isHEX, uint32_t num, uint8_t* hexStr, uint32_t size);
uint32_t ty_util_str_intStr2int(uint8_t* intStr, uint32_t size, uint32_t* num);
uint32_t ty_util_str_int2intStr(uint32_t num, uint8_t* intStr, uint32_t size);
uint32_t ty_util_str_hexStr2hexArray(uint8_t* hexStr, uint32_t size, uint8_t* hexArray);
uint32_t ty_util_str_hexArray2hexStr(bool isHEX, uint8_t* hexArray, uint32_t size, uint8_t* hexStr);


#ifdef __cplusplus
}
#endif

#endif //__TY_UTIL_H__

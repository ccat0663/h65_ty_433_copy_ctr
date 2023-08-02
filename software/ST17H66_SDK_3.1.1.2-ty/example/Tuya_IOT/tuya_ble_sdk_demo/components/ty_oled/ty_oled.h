/**
****************************************************************************
* @file      ty_oled.h
* @brief     ty_oled
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_OLED_H__
#define __TY_OLED_H__

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
#define I2C_WRITE                       0
#define I2C_READ                        1

#define SLAVE_ADDRE                     (0x3C)
#define SLAVE_ADDRE_WRITE               ((SLAVE_ADDRE<<1) | I2C_WRITE)
#define SLAVE_ADDRE_READ                ((SLAVE_ADDRE<<1) | I2C_READ)

#define WRITE_CMD_MSG                   {0x00,}
#define CMD_MSG_LEN                     2
#define WRITE_DATA_MSG                  {0x40,}
#define DATA_MSG_LEN                    2

#define OLED_CMD                        0x00
#define OLED_DATA                       0x01

#define SSD1306_SET_PAGE_ADDR           0xB0
#define SSD1306_SET_DISPLAY_START_LINE  0x40
#define SSD1306_CHARGE_PUMP             0x8D
#define SSD1306_DISPLAY_OFF             0xAE
#define SSD1306_DISPLAY_ON              0xAF
#define SSD1306_SET_CONTRAST            0x81
#define SSD1306_SEGRE_MAP               0xA1
#define SSD1306_NORMAL_DISPLAY          0xA6
#define SSD1306_SET_MULTIPLEX           0xA8
#define SSD1306_COM_SCANDEC             0xC8
#define SSD1306_SET_DISPLAY_OFFSET      0xD3
#define SSD1306_SET_DISPLAY_CLOCK_DIV   0xD5
#define SSD1306_SET_PRECHARGE           0xD9
#define SSD1306_SET_COM_PINS            0xDA
#define SSD1306_SET_VCOM_DETECT         0xDB
#define SSD1306_MEMORY_MODE             0x20
#define SSD1306_DISPLAY_ALLON_RESUME    0xA4
#define SSD1306_DEACTIVATE_SCROLL       0x2E
#define SSD1306_ACTIVATE_SCROLL         0x2f

#define SSD1306_LCD_WIDTH               128
#define SSD1306_LCD_HEIGHT              32
#define SSD1306_SET_LOW_COLUMN	        0x00
#define SSD1306_SET_HIGH_COLUMN		    0x10 //here is 0x20 not 0x10
#define SSD1306_CHARGE_PUMP_ON          0x14
#define SSD1306_CHARGE_PUMP_OFF         0x10

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
void ty_oled_init(void);
void ty_oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void ty_oled_show_string(uint8_t x, uint8_t y, char *chr, uint8_t char_size);
void ty_oled_clear(void);
void ty_oled_clear_page(uint8_t page) ;
void ty_oled_show_mac(uint8_t x, uint8_t y, uint8_t *mac);
void ty_oled_show_rssi(uint8_t x, uint8_t y,int rssi);
void ty_oled_show_string_continue(uint8_t x, uint8_t y, char *chr);


#ifdef __cplusplus
}
#endif

#endif //__TY_OLED_H__

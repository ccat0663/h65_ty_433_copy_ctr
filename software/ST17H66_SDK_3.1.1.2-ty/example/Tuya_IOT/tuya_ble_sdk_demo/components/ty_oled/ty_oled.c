#include "ty_oled.h"
#include "ty_oled_font.h"
#include "ty_i2c.h"
#include "ty_system.h"




/*********************************************************************
 * LOCAL CONSTANT
 */

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
static void i2c_write_data(uint8_t data)
{
    uint8_t cfg_msg[DATA_MSG_LEN] = WRITE_DATA_MSG;
    cfg_msg[1] = data;
    
    ty_i2c_send(SLAVE_ADDRE, cfg_msg, DATA_MSG_LEN);

//    i2c_start();
//    i2c_send_bytes(SLAVE_ADDRE_WRITE, cfg_msg, DATA_MSG_LEN);
//    i2c_stop();
}

static void i2c_write_command(uint8_t cmd)
{
    uint8_t cfg_msg[CMD_MSG_LEN] = WRITE_CMD_MSG;
    cfg_msg[1] = cmd;
    
    ty_i2c_send(SLAVE_ADDRE, cfg_msg, CMD_MSG_LEN);

//    i2c_start();
//    i2c_send_bytes(SLAVE_ADDRE_WRITE, cfg_msg, CMD_MSG_LEN );
//    i2c_stop();
}

static void oled_write_byte(uint8_t dat, uint8_t cmd)
{
	if(cmd) {
        i2c_write_data(dat);
	} else {
        i2c_write_command(dat);
	}
}

// set pos
static void oled_set_pos(uint8_t x, uint8_t y)
{ 
    oled_write_byte( SSD1306_SET_PAGE_ADDR + y, OLED_CMD ); // write page address
    oled_write_byte( (x & 0x0F), OLED_CMD );                // write column low address
	oled_write_byte( ((x & 0xF0)>>4) | 0x10, OLED_CMD );    // write column high address
}   	  

static void oled_clear_pos(uint8_t x, uint8_t y)
{ 
    oled_write_byte( SSD1306_SET_PAGE_ADDR + y, OLED_CMD ); // write page address
    oled_write_byte( (x & 0x0F), OLED_CMD );                // write column low address
	oled_write_byte( ((x & 0xF0)>>4) | 0x10, OLED_CMD );    // write column high address
}   

// display on
static void oled_display_on(void)
{
	oled_write_byte( SSD1306_CHARGE_PUMP, OLED_CMD );        // set charge pump
	oled_write_byte( SSD1306_CHARGE_PUMP_ON, OLED_CMD );     // charge pump on
	oled_write_byte( SSD1306_DISPLAY_ON, OLED_CMD );        // display on
}

// display off    
static void oled_display_off(void)
{
	oled_write_byte( SSD1306_CHARGE_PUMP, OLED_CMD);         // set charge pump
	oled_write_byte( SSD1306_CHARGE_PUMP_OFF, OLED_CMD);     // charge pump off
	oled_write_byte( SSD1306_DISPLAY_OFF, OLED_CMD);        // display off
}

// clear screen
void ty_oled_clear(void)  
{  
	uint8_t i, n;		    
	for( i = 0; i < 4; i++ ) {
		oled_write_byte ( SSD1306_SET_PAGE_ADDR + i, OLED_CMD );    //page num (0~7)
        //set oled height
		oled_write_byte ( SSD1306_SET_LOW_COLUMN, OLED_CMD );       //column low addr
		oled_write_byte ( SSD1306_SET_HIGH_COLUMN, OLED_CMD );      //column high addr

        //clear every column
		for( n = 0; n < SSD1306_LCD_WIDTH; n++ ) {
            oled_write_byte( 0, OLED_DATA );
        }
    }	
}

void ty_oled_clear_page(uint8_t page)  
{  
	uint8_t  n;		    
	oled_write_byte ( SSD1306_SET_PAGE_ADDR + page, OLED_CMD );    //page num (0~7)
    //set oled height
	oled_write_byte ( SSD1306_SET_LOW_COLUMN, OLED_CMD );       //column low addr
	oled_write_byte ( SSD1306_SET_HIGH_COLUMN, OLED_CMD );      //column high addr
		
    //clear every column
	for( n = 0; n < SSD1306_LCD_WIDTH; n++ ) {
        oled_write_byte( 0, OLED_DATA );
    }	
}

static void oled_on(void) 
{  
	uint8_t i, n;
	for( i=0; i < 4; i++ ) {
		oled_write_byte ( SSD1306_SET_PAGE_ADDR + i, OLED_CMD );    //page num (0~7)
        //set oled height
		oled_write_byte ( SSD1306_SET_LOW_COLUMN, OLED_CMD );       //column low addr
		oled_write_byte ( SSD1306_SET_HIGH_COLUMN, OLED_CMD );      //column high addr

        //clear every column
		for( n = 0; n < SSD1306_LCD_WIDTH; n++ ) {
            oled_write_byte( 0xff, OLED_DATA );
        }
    }
}

//m^n function
static uint32_t math_pow(uint8_t m,uint8_t n)
{
	uint32_t result = 1;	 
	while( n-- )
        result *= m;
	return result;
}

// show char
//x:0~127
//y:0~63	 
//size:16/6 
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t char_size)
{
	uint8_t offset = 0, i = 0;	

    // get offset from char font table
    offset = chr - ' '; 

    if( x > SSD1306_LCD_WIDTH - 1 ) {
        x = 0; 
        y = y + 2;
    }
    
    if( char_size == 16 ) { // size 8*16

        oled_set_pos( x, y );
        for(i = 0; i < 8; i++ ) {
            oled_write_byte( F8X16[offset*16+i], OLED_DATA );
        }

        oled_set_pos( x, y + 1 );
        for(i = 0; i < 8; i++ ) {
            oled_write_byte( F8X16[offset*16+i+8], OLED_DATA );
        }
    }
    else {	                // size 6*8
        oled_set_pos( x, y );
        for( i = 0; i < 6; i++ ) {
            oled_write_byte( F6x8[offset][i], OLED_DATA );
        }    
    }
}

//show numbers
//x:0~127
//y:0~63
//len :number of digits
//size:16/12
//num :(0~4294967295);	 		  
void ty_oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{         	
	uint8_t t, temp;
	uint8_t enshow = 0;

	for( t = 0; t < len; t++ ) {
		temp=( num / math_pow(10, len-t-1) ) % 10;
        
		if( enshow == 0 && t < ( len - 1 ) ) {
			if( temp == 0 ) {
				oled_show_char( x+(size/2) * t, y, ' ', size );
				continue;
			}
            else {
                enshow=1; 
            }	 
		}
	 	oled_show_char( x + (size/2) * t, y, temp+'0', size ); 
	}
}

//show string(����Ϊ8bit)
void ty_oled_show_string(uint8_t x, uint8_t y, char *chr, uint8_t char_size)
{
	uint8_t j = 0;
    
	while ( chr[j] != '\0' ) {
        oled_show_char( x, y, chr[j], char_size );
		x+=8;
		if( x > 120 ) {
            x = 0;
            y += 2;
        }
		j++;
	}
}

//show string(����Ϊ6bit)
void ty_oled_show_string_continue(uint8_t x, uint8_t y, char *chr)
{
	uint8_t j = 0;
	uint8_t offset = 0, i = 0;	
	
    
    if( x > SSD1306_LCD_WIDTH - 1 ) {
        x = 0; 
    }
	oled_set_pos( x, y );
	while ( chr[j] != '\0' ) {
    	offset = chr[j] - ' '; 
        for( i = 0; i < 6; i++ ) {
            oled_write_byte( F6x8[offset][i], OLED_DATA );
        }  
		j++;
	}
}

void ty_oled_show_mac(uint8_t x, uint8_t y, uint8_t *mac)
{
	uint8_t j = 0,i=0;
    char data[25] = "MAC:";
	for(j=0;j<6;j++){
		if(j==5)
			sprintf(data+4+i,"%02x",mac[5-j]);
		else
			sprintf(data+4+i,"%02x-",mac[5-j]);
		i+=3;
	}
	ty_oled_show_string_continue(x, y, data);
}

void ty_oled_show_rssi(uint8_t x, uint8_t y,int rssi)
{
	char data[15] = {0};
	uint8_t i=0;
	sprintf(data,"RSSI:%d ",rssi);
	ty_oled_show_string_continue(x, y, data);
}

void ssd1306_init(void)
{
#if 0
    oled_write_byte( SSD1306_DISPLAY_OFF, OLED_CMD ); 				/*display off*/
	oled_write_byte( SSD1306_DEACTIVATE_SCROLL, OLED_CMD ); 		/*scroll off*/
	oled_write_byte( SSD1306_SET_DISPLAY_START_LINE, OLED_CMD ); 	/*set display start line*/
	oled_write_byte( SSD1306_SET_PAGE_ADDR, OLED_CMD ); 			/*set page address */
	oled_write_byte( SSD1306_COM_SCANDEC, OLED_CMD ); 				/*Com scan direction*/
	oled_write_byte( SSD1306_SET_CONTRAST, OLED_CMD ); 				/*contract control*/
	oled_write_byte( 0xFF, OLED_CMD ); 								/*set max contrast*/
	oled_write_byte( SSD1306_SEGRE_MAP, OLED_CMD ); 				/*set segment remap*/
	oled_write_byte( SSD1306_NORMAL_DISPLAY, OLED_CMD ); 			/*normal / reverse*/
	oled_write_byte( SSD1306_SET_MULTIPLEX, OLED_CMD ); 			/*multiplex ratio*/
	oled_write_byte( 0x1F, 0 ); 									/*duty = 1/32*/
	oled_write_byte( SSD1306_SET_DISPLAY_OFFSET, OLED_CMD ); 		/*set display offset*/
	oled_write_byte( 0x00, OLED_CMD );
	oled_write_byte( SSD1306_SET_DISPLAY_CLOCK_DIV, OLED_CMD ); 	/*set osc division*/
	oled_write_byte( 0x80, OLED_CMD );//F0
	oled_write_byte( SSD1306_SET_PRECHARGE, OLED_CMD ); 			/*set pre-charge period*/
	oled_write_byte( 0x1f, OLED_CMD );//22
	oled_write_byte( SSD1306_SET_COM_PINS, OLED_CMD ); 				/*set COM pins*/
	oled_write_byte( 0x00, OLED_CMD );//02
	oled_write_byte( SSD1306_SET_VCOM_DETECT, OLED_CMD ); 			/*set vcomh*/
	oled_write_byte( 0x49, OLED_CMD );//49
	oled_write_byte( SSD1306_CHARGE_PUMP, OLED_CMD ); 				/*set charge pump disable*/
	oled_write_byte( SSD1306_CHARGE_PUMP_ON, OLED_CMD );
	
	oled_write_byte( SSD1306_SET_LOW_COLUMN, OLED_CMD ); 			/*set lower column address*/
	oled_write_byte( SSD1306_SET_HIGH_COLUMN, OLED_CMD ); 			/*set higher column address*/
	oled_write_byte( SSD1306_MEMORY_MODE, OLED_CMD ); 				/*Set MemoryAddressing Mode*/
	oled_write_byte( 0x02, OLED_CMD );								/*[[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;*/

	oled_write_byte( SSD1306_DISPLAY_ON, OLED_CMD ); 				/*display ON*/
	ty_oled_clear();
#endif
#if 1
	oled_write_byte(0xAE, OLED_CMD);//关闭显示
	oled_write_byte(0xAE, OLED_CMD);//关闭显示
                          
	oled_write_byte(0x40, OLED_CMD);//---set low column address
	oled_write_byte(0xB0, OLED_CMD);//---set high column address
                          
	oled_write_byte(0xC8, OLED_CMD);//-not offset
                          
	oled_write_byte(0x81, OLED_CMD);//设置对比�?
	oled_write_byte(0xff, OLED_CMD);
                          
	oled_write_byte(0xa1, OLED_CMD);//段重定向设置
                          
	oled_write_byte(0xa6, OLED_CMD);//
                          
	oled_write_byte(0xa8, OLED_CMD);//设置驱动路数
	oled_write_byte(0x1f, OLED_CMD);
                          
	oled_write_byte(0xd3, OLED_CMD);
	oled_write_byte(0x00, OLED_CMD);
                          
	oled_write_byte(0xd5, OLED_CMD);
	oled_write_byte(0xf0, OLED_CMD);
                          
	oled_write_byte(0xd9, OLED_CMD);
	oled_write_byte(0x22, OLED_CMD);
                          
	oled_write_byte(0xda, OLED_CMD);
	oled_write_byte(0x02, OLED_CMD);
                          
	oled_write_byte(0xdb, OLED_CMD);
	oled_write_byte(0x49, OLED_CMD);
                          
	oled_write_byte(0x8d, OLED_CMD);
	oled_write_byte(0x14, OLED_CMD);
                          
	oled_write_byte(0xaf, OLED_CMD);

	ty_oled_clear();
#endif
//    ty_system_delay_ms(500);
}


void ty_oled_init(void)
{
//    i2c_soft_gpio_init();
	ssd1306_init();
	ty_oled_show_string(12, 1, "Tuya BLE Demo", 16);
}







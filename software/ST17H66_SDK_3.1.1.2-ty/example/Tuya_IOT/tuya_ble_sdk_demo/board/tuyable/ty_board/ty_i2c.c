#include "i2c.h"
#include "ty_i2c.h"
#include "clock.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define IIC_SDA_PIN     P33
#define IIC_SCL_PIN     P34

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
static int hal_i2c_tx_stop(void* pi2c)
{
    AP_I2C_TypeDef* pi2cdev = (AP_I2C_TypeDef*)pi2c;

    if(pi2cdev != AP_I2C0 && pi2cdev != AP_I2C1) {
        return PPlus_ERR_INVALID_PARAM;
    }

    pi2cdev->IC_ENABLE = 0;
    
    return PPlus_SUCCESS;
}

/*********************************************************
FN: 
*/
static void ty_i2c_scan_address(void)
{
    uint8_t address;
    int err_code = 0;
    uint8_t sample_data = 0;
    bool detected_device = false;
    uint8_t tmp = 0x40;

    TY_PRINTF("TWI scanner started.");
    for (address = 1; address <= 127; address++) { // tbd
        err_code = hal_i2c_read(AP_I2C1, address, 0x40, &sample_data, sizeof(sample_data)); //0x40
        ty_i2c_send(0x3c, &tmp, 1);

        if (sample_data != PPlus_SUCCESS) { 
            detected_device = true;
            TY_PRINTF("TWI-i2c device detected at address 0x%x.", address);
        }
    }

    if (!detected_device) {
        TY_PRINTF("No device was found.");
    }

    TY_PRINTF("TWI device scan ended.");
}

/*********************************************************
FN: 
*/
static void phy_i2c_DeInit(void)
{
    hal_i2c_deinit(AP_I2C1);
    hal_gpio_pull_set(IIC_SDA_PIN,GPIO_FLOATING);  // P33 -- SDA
    hal_gpio_pull_set(IIC_SCL_PIN,GPIO_FLOATING);  // P34 -- SCL
}

/*********************************************************
FN: 
*/
uint32_t ty_i2c_init(void)
{
    hal_i2c_pin_init(I2C_1, IIC_SDA_PIN, IIC_SCL_PIN);
    hal_i2c_init(I2C_1, I2C_CLOCK_100K);  // 100k std clk speed
//    ty_i2c_scan_address();	
    return 0;
}




/*********************************************************
FN: 
*/
uint32_t ty_i2c_start(void)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_i2c_stop(void)
{
    hal_i2c_tx_stop(AP_I2C1);
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_i2c_control(uint8_t cmd, void* arg)
{
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_i2c_uninit(void)
{
    phy_i2c_DeInit();
    return 0;
}

/*********************************************************
FN: 
*/
uint32_t ty_i2c_send(const uint8_t addr, const uint8_t* buf, uint32_t size)
{
    uint8_t s_addr=addr;
    hal_i2c_addr_update(AP_I2C1, addr);
    hal_i2c_send(AP_I2C1, (void*)buf, size);
    while(hal_i2c_wait_tx_completed(AP_I2C1)){}
    return 0;
}




/*********************************************************************
 * LOCAL CONSTANT
 */
#define DELAY_CNT 1
enum {
	GPIO_INPUT_MODE,
	GPIO_OUTPUT_MODE,
};

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
void i2c_delay(unsigned long tim_1us)
{
	WaitUs(tim_1us);
//	 __NOP();
}

static void i2c_sda_pin_mode_set(uint8_t mode, uint8_t level)
{
	if(mode == GPIO_INPUT_MODE){
//		nrf_gpio_cfg_input(TY_I2C_PIN_SDA, NRF_GPIO_PIN_PULLUP);
	} else if(mode == GPIO_OUTPUT_MODE){
//        nrf_gpio_cfg_output(TY_I2C_PIN_SDA);
	}
}

static void i2c_sda_pin_set(uint8_t level)
{
//    nrf_gpio_pin_write(TY_I2C_PIN_SDA, level);
}

static void i2c_scl_pin_set(uint8_t level)
{
//    nrf_gpio_pin_write(TY_I2C_PIN_SCL, level);
}

static uint8_t i2c_sda_pin_status_get(void)
{
//    return nrf_gpio_pin_read(TY_I2C_PIN_SDA)?1:0;
    return 0;
}

/**
 * @description: i2c ack func
 * @param {type} none
 * @return: none
 */
static void i2c_ack(void)
{
    i2c_scl_pin_set(0);
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);
    
    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
}

/**
 * @description: i2c none ack func
 * @param {type} none
 * @return: none
 */
static void i2c_noack(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );
    i2c_sda_pin_set( 1 );

    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);
    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
}

/**
 * @description: i2c wait ack
 * @param {type} none
 * @return: rev ack return true else return false
 */
static uint8_t i2c_wait_ack(void)
{
    uint8_t cnt = 50;

    i2c_sda_pin_mode_set( GPIO_INPUT_MODE, 1 );/* set input and release SDA */
    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 0 );       /* put down SCL ready to cheack SCA status */
    i2c_delay(DELAY_CNT);
    
    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);
    
    while( i2c_sda_pin_status_get() ) /* get ack */
    {
        cnt--;
        if( cnt==0 )
        {
            i2c_scl_pin_set( 0 );
            return false;
        }
        i2c_delay(DELAY_CNT);
    }
    
    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
    return true;
}

/**
 * @description: i2c start signal
 * @param {type} none
 * @return: none
 */
void i2c_start(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );    //SDA output mode

    i2c_scl_pin_set( 1 );
    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);
  
    i2c_scl_pin_set( 0 );
    i2c_delay(DELAY_CNT);
}

/**
 * @description: i2c stop signal
 * @param {type} none
 * @return: none
 */
void i2c_stop(void)
{
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 0 );     //SDA input mode

    i2c_scl_pin_set( 0 );
    i2c_sda_pin_set( 0 );
    i2c_delay(DELAY_CNT);

    i2c_scl_pin_set( 1 );
    i2c_delay(DELAY_CNT);

    i2c_sda_pin_set( 1 );
    i2c_delay(DELAY_CNT);
}

/**
 * @description: send one byte to i2c bus
 * @param {uint8_t} data send to i2c
 * @return: none
 */
void i2c_send_byte(uint8_t data)
{
    uint8_t idx = 0;
    i2c_scl_pin_set( 0 );
    i2c_sda_pin_mode_set( GPIO_OUTPUT_MODE, 1 );
    
    for( idx=0; idx<8; idx++ ) {
        if( data & 0x80 ) {
            i2c_sda_pin_set( 1 );
        } else {
            i2c_sda_pin_set( 0 );
        }
        i2c_delay(DELAY_CNT);
        
        i2c_scl_pin_set( 1 );
        i2c_delay(DELAY_CNT);
        
        i2c_scl_pin_set( 0 );
        i2c_delay(DELAY_CNT);
        
        data <<= 1;
    }
}

/**
 * @description: send bytes to i2c bus
 * @param {type} none
 * @return: none
 */
void i2c_send_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();

    for( idx=0; idx<len; idx++ ) {
        i2c_send_byte( buff[idx] );
        i2c_wait_ack();
    }
}

/**
 * @description: recive one byte from i2c bus
 * @param {type} none
 * @return: none
 */
void i2c_rcv_byte(uint8_t *data)
{
    uint8_t idx;
    i2c_sda_pin_mode_set( GPIO_INPUT_MODE, 1 );
    i2c_delay(25);
    
    for( idx=0; idx<8; idx++ ) {
        i2c_scl_pin_set( 0 );
        i2c_delay(DELAY_CNT);

        i2c_scl_pin_set( 1 );
        *data = *data << 1;
        if( i2c_sda_pin_status_get() ) {
            *data |= 1;
        }
        i2c_delay(DELAY_CNT);
    }
    
    i2c_scl_pin_set( 0 );
}

/**
 * @description: recive bytes from i2c bus,last byte none ack
 * @param {type} none
 * @return: none
 */
void i2c_rcv_bytes(uint8_t adderss_cmd, uint8_t *buff, uint8_t len)
{
    uint8_t idx;
    i2c_send_byte( adderss_cmd );
    i2c_wait_ack();
    
    for( idx=0; idx<len; idx++ ) {
        i2c_rcv_byte( &buff[idx] );
        
        if( idx<len-1 ) {
            i2c_ack();
        } else {
            i2c_noack();
        }
    }
}

void i2c_soft_cfg(uint8_t adderss_cmd,uint8_t reg_addr,uint8_t data)
{
    i2c_start();
    i2c_send_byte(adderss_cmd);
    i2c_wait_ack();
    i2c_send_byte(reg_addr);
    i2c_wait_ack();
    i2c_send_byte(data);
    i2c_wait_ack();		
    i2c_stop();
}

void i2c_write_reg(uint8_t address_cmd,uint8_t register_addr)
{
	i2c_start();
    i2c_send_bytes(address_cmd,&register_addr ,1);
    i2c_stop();	
}

void i2c_soft_gpio_init(void)
{
//	nrf_gpio_cfg_output(TY_I2C_PIN_SDA);
//	nrf_gpio_cfg_output(TY_I2C_PIN_SCL);
}












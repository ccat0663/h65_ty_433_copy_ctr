#include "ty_spi.h"
#include "spi.h"




/*********************************************************************
 * LOCAL CONSTANT
 */
#define TY2_SPI_PIN_MISO         P18
#define TY2_SPI_PIN_MOSI         P20
#define TY2_SPI_PIN_CLK          P31
#define TY2_SPI_PIN_CSN          P32

/*********************************************************************
 * LOCAL STRUCT
 */

/*********************************************************************
 * LOCAL VARIABLE
 */
static hal_spi_t ty_spi_ptr = {
    .spi_index = SPI1,
};

static spi_Cfg_t s_spim_cfg = {
    .sclk_pin = TY2_SPI_PIN_CLK,
    .ssn_pin = TY2_SPI_PIN_CSN,
    .MOSI = TY2_SPI_PIN_MOSI,
    .MISO = TY2_SPI_PIN_MISO,

    .baudrate = 1000000,
    .spi_tmod = SPI_TRXD,
    .spi_scmod = SPI_MODE0,

    .int_mode = false,
    .force_cs = true,
    .evt_handler = NULL,
};

static volatile bool s_transferOk = true;

/*********************************************************************
 * VARIABLE
 */

/*********************************************************************
 * LOCAL FUNCTION
 */




/*********************************************************
FN: 
*/
void ty_spi_init(void)
{
    hal_spi_init(SPI1);
}

/*********************************************************
FN: 
*/
void ty_spi_csn_set(bool pinState)
{
    if(pinState == TY_SPI_CS_OFF) {
        hal_gpio_write(TY2_SPI_PIN_CSN, 1);
    } else if(pinState == TY_SPI_CS_ON) {
        hal_gpio_write(TY2_SPI_PIN_CSN, 0);
    }
}

/*********************************************************
FN: 
*/
void ty_spi_readWriteData(uint8_t *pWriteData, uint8_t *pReadData, uint8_t writeDataLen)
{
    int ret = hal_spi_transmit(&ty_spi_ptr, SPI_TRXD, pWriteData, pReadData, writeDataLen, writeDataLen);
    if(ret != PPlus_SUCCESS) {
        TY_PRINTF("err ret: %0x \n", ret);
    } else {
        TY_PRINTF("trx ok\n");
    }
}

/*********************************************************
FN: Turn on SPI, which is different from initialization: CS pin is not initialized
*/
void ty_spi_enable(void)
{
    hal_spi_bus_init(&ty_spi_ptr, s_spim_cfg);
}

/*********************************************************
FN: 
*/
void ty_spi_disable(void)
{
    hal_spi_bus_deinit(&ty_spi_ptr);
}
















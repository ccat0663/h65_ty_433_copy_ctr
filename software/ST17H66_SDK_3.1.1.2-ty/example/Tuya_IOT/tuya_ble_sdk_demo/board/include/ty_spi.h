/**
****************************************************************************
* @file      ty_spi.h
* @brief     ty_spi
* @author    suding
* @version   V1.0.0
* @date      2020-10
* @note
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2020 Tuya </center></h2>
*/


#ifndef __TY_SPI_H__
#define __TY_SPI_H__

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
#define TY_SPI_PIN_MISO         14
#define TY_SPI_PIN_MOSI         15
#define TY_SPI_PIN_CLK          12
#define TY_SPI_PIN_CSN          19

#define TY_SPI_CS_ON            0
#define TY_SPI_CS_OFF           1

#define TY_SPI_CS_HIGH          ty_spi_csn_set(TY_SPI_CS_OFF)
#define TY_SPI_CS_LOW           ty_spi_csn_set(TY_SPI_CS_ON)

/*********************************************************************
 * STRUCT
 */

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */
void ty_spi_init(void);
void ty_spi_csn_set(bool pinState);
void ty_spi_readWriteData(uint8_t *pWriteData, uint8_t *pReadData, uint8_t writeDataLen);
void ty_spi_enable(void);
void ty_spi_disable(void);


#ifdef __cplusplus
}
#endif

#endif //__TY_SPI_H__

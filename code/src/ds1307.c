/**
  ******************************************************************************
  * @file    ds1307.c 
  * @author  Pablo Fuentes
	* @version V1.0.0
  * @date    2018
  * @brief   DS1307 Functions
  ******************************************************************************
*/

#include "ds1307.h"

/** 
 ===============================================================================
              ##### DEFINICIONES #####
 ===============================================================================
 */

#define DS1307_ADDR 0xD0 //0x68 << 1

typedef struct
{
  I2C_TypeDef *I2Cx;
} ds1307_t;

static ds1307_t ds;
static char tx_i2c_data[9];
static char rx_i2c_data[20];

/** 
 ===============================================================================
              ##### Static functions #####
 ===============================================================================
 */

static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

/** 
 ===============================================================================
              ##### Public functions #####
 ===============================================================================
 */

void ds1307_init(I2C_TypeDef *I2Cx)
{
  ds.I2Cx = I2Cx;
}

bool ds1307_adjust(bool en_1hz_out, uint8_t d, uint8_t mt, uint16_t y, uint8_t h, uint8_t m, uint8_t s)
{
  tx_i2c_data[0] = 0x00;
  tx_i2c_data[1] = bin2bcd(s) & 0x7F;
  tx_i2c_data[2] = bin2bcd(m);
  tx_i2c_data[3] = bin2bcd(h) & 0x3F;
  tx_i2c_data[4] = 0;
  tx_i2c_data[5] = bin2bcd(d);
  tx_i2c_data[6] = bin2bcd(mt);
  tx_i2c_data[7] = bin2bcd(y - 2000);
  if (!en_1hz_out)
    tx_i2c_data[8] = 0x00;
  else
    tx_i2c_data[8] = 0x10;
  if (i2c_write(ds.I2Cx, DS1307_ADDR, tx_i2c_data, 9, I2C_STOP) == 9)
    return true;
  else
    return false;
}

bool ds1307_adjustUnix(bool en_1hz_out, uint32_t unix)
{
  LocalTime_t lt;
  unix2time(unix, &lt);
  tx_i2c_data[0] = 0x00;
  tx_i2c_data[1] = bin2bcd(lt.seconds) & 0x7F;
  tx_i2c_data[2] = bin2bcd(lt.minutes);
  tx_i2c_data[3] = bin2bcd(lt.hours) & 0x3F;
  tx_i2c_data[4] = 0;
  tx_i2c_data[5] = bin2bcd(lt.day);
  tx_i2c_data[6] = bin2bcd(lt.month);
  tx_i2c_data[7] = bin2bcd(lt.year);
  if (!en_1hz_out)
    tx_i2c_data[8] = 0x00;
  else
    tx_i2c_data[8] = 0x10;
  if (i2c_write(ds.I2Cx, DS1307_ADDR, tx_i2c_data, 9, I2C_STOP) == 9)
    return true;
  else
    return false;
}

bool ds1307_now(LocalTime_t *lt)
{
  tx_i2c_data[0] = 0;
  if (i2c_write(ds.I2Cx, DS1307_ADDR, tx_i2c_data, 1, I2C_STOP) != 1)
    return false;

  i2c_read(ds.I2Cx, DS1307_ADDR, rx_i2c_data, 7, I2C_STOP);
  lt->day = bcd2bin(rx_i2c_data[4]);
  lt->month = bcd2bin(rx_i2c_data[5]);
  lt->year = bcd2bin(rx_i2c_data[6]);
  lt->hours = bcd2bin(rx_i2c_data[2]);
  lt->minutes = bcd2bin(rx_i2c_data[1]);
  lt->seconds = bcd2bin(rx_i2c_data[0] & 0x7F);

  return true;
}

uint32_t ds1307_nowUnix(void)
{
  uint32_t result = 0;
  int8_t tries = 10;
  tx_i2c_data[0] = 0;

  do
  {
    if (i2c_write(ds.I2Cx, DS1307_ADDR, tx_i2c_data, 1, I2C_STOP) != 1)
      continue;
    i2c_read(ds.I2Cx, DS1307_ADDR, rx_i2c_data, 7, I2C_STOP);
    result = time2unix(bcd2bin(rx_i2c_data[4]), bcd2bin(rx_i2c_data[5]), bcd2bin(rx_i2c_data[6]),
                       bcd2bin(rx_i2c_data[2]), bcd2bin(rx_i2c_data[1]), bcd2bin(rx_i2c_data[0] & 0x7F));
  } while (result == 0 && tries--);

  return result;
}

void ds1307_printTime(LocalTime_t *lt)
{
  lprint("{D}/{D}/{D} {D}:{D}:{D}\n", lt->day,
         lt->month,
         lt->year,
         lt->hours,
         lt->minutes,
         lt->seconds);
}

void ds1307_printUnix(uint32_t unix)
{
  LocalTime_t lt;
  unix2time(unix, &lt);
  lprint("{D}/{D}/{D} {D}:{D}:{D}\n", lt.day,
         lt.month,
         lt.year,
         lt.hours,
         lt.minutes,
         lt.seconds);
}
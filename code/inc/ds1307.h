/**
  ******************************************************************************
  * @file    ds1307.h 
  * @author  Pablo Fuentes
	* @version V1.0.0
  * @date    2018
  * @brief   DS1307 Library
  ******************************************************************************
*/

#ifndef __DS1307_H
#define __DS1307_H

#include "eonOS.h"

/** 
 ===============================================================================
              ##### Public functions #####
 ===============================================================================
 */

void ds1307_init(I2C_TypeDef *I2Cx);
bool ds1307_adjust(bool en_1hz_out, uint8_t d, uint8_t mt, uint16_t y, uint8_t h, uint8_t m, uint8_t s);
bool ds1307_adjustUnix(bool en_1hz_out, uint32_t unix);
bool ds1307_now(LocalTime_t *lt);
uint32_t ds1307_nowUnix(void);
void ds1307_printTime(LocalTime_t *lt);
void ds1307_printUnix(uint32_t unix);

#endif
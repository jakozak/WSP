/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       HSHCAL.c
 *
 *  @brief      Driver for the ALPS HSHCAL001B humidity and temperature sensor.
 *  ============================================================================
 */

/* -----------------------------------------------------------------------------
*  Includes
* ------------------------------------------------------------------------------
*/
#include "Board.h"
#include "SensorHSHCAL.h"
#include "SensorUtil.h"
#include "SensorI2C.h"
#include "math.h"

/* -----------------------------------------------------------------------------
*  Constants and macros
* ------------------------------------------------------------------------------
*/

/* Register addresses MSB first */
#define REG_UVB		                    0x10	//UVB output 1 - low byte
#define REG_STAT						0x18	//Status register
#define REG_CTL1		                0x1B	//Control register
#define REG_CTL2		                0x1C	//Control register
#define REG_CTL3		                0x1D	//Control register
#define REG_CTL4		                0x1E	//Control register

#define REG_STBA	                    0x0B	//Self-test A response
#define REG_STBB	                    0x0C	//Self-test B response
#define REG_INF			                0x0D	//Information register
#define REG_WIA			                0x0F	//"Who I Am" Register

/* Register values little endian*/
#define INF_VAL		                	0x4554  //0x5445
#define WIA_VAL                         0x49

/* Bit values */
#define CTL1_MEASURE_ENABLE           	0xA2 	//Active mode, 12bit resolution, Reverse input subtraction drive, 10Hz, Force state
#define CTL1_MEASURE_DISABLE           	0x22	//Stand-by mode, 12bit resolution, Reverse input subtraction drive, 10Hz, Force state
#define CTL3_MEASURE_ENABLE				0x40	//Start to measure in Force State

/* Bit values */
#define STAT_DATA_RDY                   0x48

/* Register length */
#define REGISTER_LENGTH                 1

/* Sensor data size */
#define DATA_LENGTH                     2

// Sensor selection/de-selection
#define SENSOR_SELECT()     SensorI2C_select(SENSOR_I2C_0,Board_HSUDDD_ADDR)
#define SENSOR_DESELECT()   SensorI2C_deselect()

/* -----------------------------------------------------------------------------
*  Public functions
* ------------------------------------------------------------------------------
*/

void SensorHsuddd_force_measurement(void)
{
	bool success;
	uint16_t val;

    if (!SENSOR_SELECT())
    {
        return;
    }

    val = CTL1_MEASURE_ENABLE;

    success = SensorI2C_writeReg(REG_CTL1, (uint8_t *)&val, REGISTER_LENGTH);

    if(success)
    {
    val = CTL3_MEASURE_ENABLE;

    SensorI2C_writeReg(REG_CTL3, (uint8_t *)&val, REGISTER_LENGTH);
    }
    SENSOR_DESELECT();
}

/*******************************************************************************
 * @fn          SensorOpt3001_read
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      true if valid data
 ******************************************************************************/
bool SensorHsuddd_read(uint16_t *rawData)
{
    bool success;
    uint16_t val=0;

    if (!SENSOR_SELECT())
    {
        return false;
    }

    success = SensorI2C_readReg(REG_STAT, (uint8_t *)&val, REGISTER_LENGTH);

    if (success)
    {
      //  success = val == STAT_DATA_RDY;
    }

    if (success)
    {
        success = SensorI2C_readReg(REG_UVB, (uint8_t*)&val, DATA_LENGTH);

    }

	if (success)
    {
        // Swap bytes
        *rawData = val;

    }

    SENSOR_DESELECT();

    return success;
}


/*******************************************************************************
 * @fn          SensorOpt3001_test
 *
 * @brief       Run a sensor self-test
 *
 * @return      true if passed
 ******************************************************************************/
/*bool SensorHsuddd_test(void)
{
    uint16_t val;

    // Select this sensor on the I2C bus
    if (!SENSOR_SELECT())
    {
        return false;
    }

    // Check manufacturer ID
//    ST_ASSERT(SensorI2C_readReg(REG_MANUFACTURER_ID, (uint8_t *)&val, REGISTER_LENGTH));
//    ST_ASSERT(val == MANUFACTURER_ID);

    // Check device ID
//    ST_ASSERT(SensorI2C_readReg(REG_DEVICE_ID, (uint8_t *)&val, REGISTER_LENGTH));
 //   ST_ASSERT(val == DEVICE_ID);

    SENSOR_DESELECT();

    return true;
}
*/
/*******************************************************************************
 * @fn          SensorOpt3001_convert
 *
 * @brief       Convert raw data to LUX
 *
 * @param       rawData - raw data from sensor
 *
 * @return      lux - converted value (lux)
 *
 ******************************************************************************/
float SensorHsuddd_convert(uint16_t rawData)
{


    return 0.000852*rawData;
}

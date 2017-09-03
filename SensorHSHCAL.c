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
#define REG_HUMI1                       0x10	//Humidity output 1 - low byte
#define REG_HUMI2                       0x11	//Humidity output 2 - high byte
#define REG_TEMP1                       0x12	//Temperature output 1 - low byte
#define REG_TEMP2                       0x13	//Temperature output 2 - high byte
#define REG_STAT						0x18	//Status register
#define REG_CTL1		                0x1B	//Control register

#define REG_STR		                    0x0C	//Self test response
#define REG_INF1		                0x0D	//The version of IC and customer code
#define REG_INF2		                0x0E	//A sensor category and a product version
#define REG_WIA			                0x0F	//ID of a supplier

#define REG_REST	                    0x30	//Reset action command
#define REG_DET		                    0x31	//Start measurment in force state command
#define REG_STST	                    0x32	//Self test command

/* Register values */
#define INF1_VAL		                0x11
#define INF2_VAL                        0x23
#define WIA_VAL                         0x49

#define CONFIG_ENABLE                   0x40
#define CONFIG_DISABLE                  0x42

/* Bit values */
#define DATA_RDY_BIT                    0x42

/* Register length */
#define REGISTER_LENGTH                 1

/* Sensor data size */
#define DATA_LENGTH                     1

// Sensor selection/de-selection
#define SENSOR_SELECT()     SensorI2C_select(SENSOR_I2C_0,Board_HSHCAL_ADDR)
#define SENSOR_DESELECT()   SensorI2C_deselect()

/* -----------------------------------------------------------------------------
*  Public functions
* ------------------------------------------------------------------------------
*/

/*******************************************************************************
 * @fn          SensorOpt3001_init
 *
 * @brief       Initialize the temperature sensor driver
 *
 * @return      none
 ******************************************************************************/
bool SensorHshcal_init(void)
{
    SensorHshcal_enable(false);

    return true;
}

/*******************************************************************************
 * @fn          SensorOpt3001_enable
 *
 * @brief       Turn the sensor on/off
 *
 * @return      none
 ******************************************************************************/
void SensorHshcal_enable(bool enable)
{
    uint16_t val;

    if (!SENSOR_SELECT())
    {
        return;
    }

    if (enable)
    {
        val = CONFIG_ENABLE;
    }
    else
    {
        val = CONFIG_DISABLE;
    }

    SensorI2C_writeReg(REG_CTL1, (uint8_t *)&val, REGISTER_LENGTH);

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
bool SensorHshcal_temp_read(uint16_t *rawData)
{
    bool success;
    uint16_t val1=0, val2=0, val=0;
    uint16_t res;

    if (!SENSOR_SELECT())
    {
        return false;
    }

    /*success = SensorI2C_readReg(REG_CONFIGURATION, (uint8_t *)&val, REGISTER_LENGTH);

    if (success)
    {
        success = (val & DATA_RDY_BIT) == DATA_RDY_BIT;
    }
	*/
    //if (success)
   // {
        success = SensorI2C_readReg(REG_TEMP1, (uint8_t*)&val1, DATA_LENGTH);
   // }

	if (success)
	{
		success = SensorI2C_readReg(REG_TEMP2, (uint8_t*)&val2, DATA_LENGTH);
	}

	if (success)
    {
        // Swap bytes
        //*rawData = (val << 8) | (val>>8 &0xFF);
    	res=val2;
    	res=(res<<8) | val1;
		*rawData=res;
    }

    SENSOR_DESELECT();

    return success;
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
bool SensorHshcal_humi_read(uint16_t *rawData)
{
    bool success;
    uint8_t val1, val2;
    uint16_t res;

    if (!SENSOR_SELECT())
    {
        return false;
    }

    /*success = SensorI2C_readReg(REG_CONFIGURATION, (uint8_t *)&val, REGISTER_LENGTH);

    if (success)
    {
        success = (val & DATA_RDY_BIT) == DATA_RDY_BIT;
    }
	*/
    //if (success)
   // {
        success = SensorI2C_readReg(REG_HUMI1, (uint8_t*)&val1, DATA_LENGTH);
   // }

	if (success)
	{
		success = SensorI2C_readReg(REG_HUMI2, (uint8_t*)&val2, DATA_LENGTH);
	}

	if (success)
    {
        // Swap bytes
        //*rawData = (val << 8) | (val>>8 &0xFF);
    	res=val2;
    	res=(res<<8) | val1;
		*rawData=res;
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
bool SensorHshcal_test(void)
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
float SensorHshcal_humi_convert(uint16_t rawData)
{


    return 0.015625*rawData-14;
}
float SensorHshcal_temp_convert(uint16_t rawData)
{


    return 0.02*rawData-41.92;
}

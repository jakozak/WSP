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
#define CMD_START	                    0xAC	//Start measurement command

/* Bit values */
#define STATUS_OK	                    0x60	//Data are correct

/* Register length */
#define REGISTER_LENGTH                 1

/* Sensor data size */
#define DATA_LENGTH                     5

// Sensor selection/de-selection
#define SENSOR_SELECT()     SensorI2C_select(SENSOR_I2C_0,Board_HSPPAD_ADDR)
#define SENSOR_DESELECT()   SensorI2C_deselect()

/* -----------------------------------------------------------------------------
*  Public functions
* ------------------------------------------------------------------------------
*/
/*******************************************************************************
 * @fn          SensorOpt3001_read
 *
 * @brief       Read the result register
 *
 * @param       Buffer to store data in
 *
 * @return      true if valid data
 ******************************************************************************/

bool SensorHsppad_read(uint8_t *rawData)
{
    bool success;
    uint8_t val[5];
    uint8_t i=0;

    if (!SENSOR_SELECT())
    {
        return false;
    }

    success = SensorI2C_readReg(CMD_START, (uint8_t*)&val, DATA_LENGTH);

	if (success)
	{
		success = val[0] == STATUS_OK;
	}

	if (success)
    {

		for(i=0; i<DATA_LENGTH; i++)
		{
		*rawData=val[i];
		rawData++;
		}

    }

    SENSOR_DESELECT();

    return success;
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
//float SensorHsppad_pres_convert(uint8_t *rawData_pointer)
//{
//	uint16_t rawData;
//	rawData=*(rawData_pointer+1) << 8 | *(rawData_pointer+2);
//
//    return 0.0131228*rawData+250;
//}

float SensorHsppad_pres_convert(uint16_t rawData)
{
	    return 0.0131228*rawData+250;
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
//float SensorHsppad_temp_convert(uint8_t *rawData_pointer)
//{
//	uint16_t rawData;
//	rawData=*(rawData_pointer+3) << 8 | *(rawData_pointer+4);
//
//	return 0.0019074*rawData-40;
//}
float SensorHsppad_temp_convert(uint16_t rawData)
{
		return 0.0019074*rawData-40;
}


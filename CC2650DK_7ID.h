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
 *  @file       Board.h
 *
 *  @brief      CC2650EM_7ID Board Specific header file.
 *              The project options should point to this file if this is the
 *              CC2650EM you are developing code for.
 *
 *  The CC2650 header file should be included in an application as follows:
 *  @code
 *  #include <Board.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC2650EM_7ID_H__
#define __CC2650EM_7ID_H__

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *  Symbol by generic Board.c to include the correct kit specific Board.c
 *  ==========================================================================*/
#define CC2650EM_7ID


/** ============================================================================
 *  Includes
 *  ==========================================================================*/
#include <ti/drivers/PIN.h>
#include <driverlib/ioc.h>

/** ============================================================================
 *  Externs
 *  ==========================================================================*/
extern const PIN_Config BoardGpioInitTable[];

/** ============================================================================
 *  Defines
 *  ==========================================================================*/

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>                <pin mapping>
 */
/* Leds */
#define Board_LED_ON                        1
#define Board_LED_OFF                       0
#define Board_LED                       	IOID_7
/* Button Board */
#define Board_BUTTON       		            IOID_5
/* UART Board */
#define Board_UART_RX                       IOID_9
#define Board_UART_TX                       IOID_10
#define Board_UART_CTS                      IOID_UNUSED
#define Board_UART_RTS                      IOID_UNUSED
/*XBEE module*/
#define Board_XBEE_SLP                      IOID_12
#define Board_VTRANS_EN						IOID_8
#define Board_SLEEP_IND                     IOID_13
#define Board_ASSCT							IOID_14
/* I2C Board*/
#define Board_I2C0_SDA0             		IOID_24
#define Board_I2C0_SCL0             		IOID_25
/* LDO enable pin */
#define Board_1V8_EN						IOID_27
#define Board_3V3_EN						IOID_26
/*I2C sensors addresses*/
#define     Board_OPT3001_ADDR      0x44
#define		Board_HSHCAL_ADDR		0x18
#define		Board_HSPPAD_ADDR		0x48
#define		Board_HSUDDD_ADDR		0xD

/** ============================================================================
 *  Instance identifiers
 *  ==========================================================================*/
/* Generic SPI instance identifiers */
#define Board_SPI0                  CC2650DK_7ID_SPI0
#define Board_SPI1                  CC2650DK_7ID_SPI1
/* Generic I2C instance identifiers */
#define Board_I2C                   CC2650DK_7ID_I2C0
/* Generic UART instance identifiers */
#define Board_UART                  CC2650DK_7ID_UART0
/* Generic Crypto instance identifiers */
#define Board_CRYPTO                CC2650DK_7ID_CRYPTO0

/** ============================================================================
 *  Number of peripherals and their names
 *  ==========================================================================*/

/*!
 *  @def    CC2650DK_7ID_I2CName
 *  @brief  Enum of I2C names on the CC2650 dev board
 */
typedef enum CC2650DK_7ID_I2CName {
    CC2650DK_7ID_I2C0 = 0,

    CC2650DK_7ID_I2CCOUNT
} CC2650STK_I2CName;


/*!
 *  @def    CC2650DK_7ID_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
typedef enum CC2650DK_7ID_CryptoName {
    CC2650DK_7ID_CRYPTO0 = 0,
    CC2650DK_7ID_CRYPTOCOUNT
} CC2650DK_7ID_CryptoName;

/*!
 *  @def    CC2650DK_7ID_SPIName
 *  @brief  Enum of SPI names on the CC2650 dev board
 */
typedef enum CC2650DK_7ID_SPIName {
    CC2650DK_7ID_SPI0 = 0,
    CC2650DK_7ID_SPI1,
    CC2650DK_7ID_SPICOUNT
} CC2650DK_7ID_SPIName;

/*!
 *  @def    CC2650DK_7ID_UARTName
 *  @brief  Enum of UARTs on the CC2650 dev board
 */
typedef enum CC2650DK_7ID_UARTName {
    CC2650DK_7ID_UART0 = 0,
    CC2650DK_7ID_UARTCOUNT
} CC2650DK_7ID_UARTName;

/*!
 *  @def    CC2650DK_7ID_UdmaName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC2650DK_7ID_UdmaName {
    CC2650DK_7ID_UDMA0 = 0,
    CC2650DK_7ID_UDMACOUNT
} CC2650DK_7ID_UdmaName;

#ifdef __cplusplus
}
#endif

#endif /* __CC2650EM_H__ */

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

/*
 *  ======== empty_min.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
#include <driverlib/aux_adc.h>

#include <inc/hw_fcfg1.h>

/* Board Header files */
#include "Board.h"

/*Sensor Controller Header file */
#include "scif.h"

#define BV(n)               (1 << (n))

// Display error message if the SCIF driver has been generated with incorrect operating system setting
#ifndef SCIF_OSAL_TIRTOS_H
    #error "SCIF driver has incorrect operating system configuration for this example. Please change to 'TI-RTOS' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

// Display error message if the SCIF driver has been generated with incorrect target chip package
#ifndef SCIF_TARGET_CHIP_PACKAGE_QFN48_7X7_RGZ
    #error "SCIF driver has incorrect target chip package configuration for this example. Please change to 'QFN48 7x7 RGZ' in the Sensor Controller Studio project panel and re-generate the driver."
#endif

// Task data
Task_Struct readTask, sendTask;
Char readTaskStack[1024], sendTaskStack[1024];

// Semaphore used to wait for Sensor Controller task ALERT event
static Semaphore_Struct semScTaskAlert;
// Semaphore used to wait for network assotiation of Xbee
static Semaphore_Struct semAssct;



uint8_t i=0;
uint32_t j=0;

void scCtrlReadyCallback(void) {

} // scCtrlReadyCallback


void scTaskAlertCallback(void) {

    // Wake up the OS task
    Semaphore_post(Semaphore_handle(&semScTaskAlert));

} // scTaskAlertCallback

/// Output data structure
#pragma pack(push, 1)
typedef struct {
	uint32_t deviceID;			///< Device ID
	uint16_t measurementStatus;	///< I2C measurement status. 0 - OK, 1 - error
	uint16_t hshcalHumiValue;  	///< HSHCAL humidity measurement result
    uint16_t hshcalTempValue;  	///< HSHCAL temperature measurement result
    uint16_t hsppadPressValue; 	///< HSPPAD pressure measurement result
    uint16_t hsppadTempValue;  	///< HSPPAD temperature measurement result
    uint16_t hsudddUvbValue;   	///< HSUDDD UVB measurement result
    uint16_t illuValue;        	///< Opt3001 illuminance measurement result
    int32_t adcValue;         	///< ADC Value
    int32_t batteryValue;       ///< Battery voltage through resistor divider
} UART_OUTPUT_STRUCTURE;
#pragma pack(pop)

UART_OUTPUT_STRUCTURE scOutputData;


/* Pin driver handle */
static PIN_Handle 	xbeePinHandle, ledPinHandle, interruptSleepHandle, interruptAssctHandle;
static PIN_State 	xbeePinState, ledPinState, interruptSleepState, interruptAssctState;



/*
 * 	Xbee module sleep control pin.

 */
PIN_Config ledPinTable[] = {
		Board_LED 		 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH   | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

PIN_Config xbeePinTable[] = {
	Board_XBEE_SLP   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW   | PIN_PUSHPULL | PIN_DRVSTR_MIN,
	Board_3V3_EN   	 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW   | PIN_PUSHPULL | PIN_DRVSTR_MIN,
	Board_VTRANS_EN  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW   | PIN_PUSHPULL | PIN_DRVSTR_MIN,
	PIN_TERMINATE
};

PIN_Config interruptSleepTable[] = {
	Board_SLEEP_IND	 | PIN_INPUT_EN 	  | PIN_NOPULL		| PIN_IRQ_DIS,
	PIN_TERMINATE
};

PIN_Config interruptAssctTable[] = {
	Board_ASSCT		 | PIN_INPUT_EN 	  | PIN_NOPULL		| PIN_IRQ_DIS,
	PIN_TERMINATE
};

void taskReadFxn(UArg a0, UArg a1) {

//	int32_t adcOffset = AUXADCGetAdjustmentOffset(AUXADC_REF_FIXED);
//	int32_t adcGainError = AUXADCGetAdjustmentGain(AUXADC_REF_FIXED);
//	int32_t adcCorrectedValue, batteryCorrectedValue;

    // Initialize the Sensor Controller
    scifOsalInit();
    scifOsalRegisterCtrlReadyCallback(scCtrlReadyCallback);
    scifOsalRegisterTaskAlertCallback(scTaskAlertCallback);
    scifInit(&scifDriverSetup);

    // Set the Sensor Controller task tick interval to 20 second
    //scifStartRtcTicks(0x00020000, 0x02580000); //10 min
    scifStartRtcTicks(0x00020000, 0x00140000); //20 sec

    // Configure to trigger interrupt at first result, and start the Sensor Controller's I2C Light
    // Sensor task (not to be confused with OS tasks)
    scifStartTasksNbl(BV(SCIF_I2C_AND_ADC_SENSORS_TASK_ID));

    // Main loop
    while (1) {

        // Wait for an ALERT callback
        Semaphore_pend(Semaphore_handle(&semScTaskAlert), BIOS_WAIT_FOREVER);

        // Clear the ALERT interrupt source
        scifClearAlertIntSource();

        PIN_setOutputValue(ledPinHandle, Board_LED, Board_LED_OFF);
        PIN_setOutputValue(xbeePinHandle, Board_3V3_EN, 1);
		PIN_setOutputValue(xbeePinHandle, Board_XBEE_SLP, 0);

        //batteryCorrectedValue = AUXADCAdjustValueForGainAndOffset((int32_t) scifTaskData.i2cAndAdcSensors.output.batteryValue, adcGainError, adcOffset);
        //adcCorrectedValue = AUXADCAdjustValueForGainAndOffset((int32_t) scifTaskData.i2cAndAdcSensors.output.adcValue, adcGainError, adcOffset);
        scOutputData.measurementStatus = scifTaskData.i2cAndAdcSensors.state.i2cStatus;
		scOutputData.hshcalHumiValue = scifTaskData.i2cAndAdcSensors.output.hshcalHumiValue;
		scOutputData.hshcalTempValue = scifTaskData.i2cAndAdcSensors.output.hshcalTempValue;
		scOutputData.hsppadPressValue = scifTaskData.i2cAndAdcSensors.output.hsppadPressValue;
		scOutputData.hsppadTempValue = scifTaskData.i2cAndAdcSensors.output.hsppadTempValue;
		scOutputData.hsudddUvbValue = scifTaskData.i2cAndAdcSensors.output.hsudddUvbValue;
		scOutputData.illuValue = scifTaskData.i2cAndAdcSensors.output.illuValue;
		scOutputData.adcValue = AUXADCValueToMicrovolts(AUXADC_FIXED_REF_VOLTAGE_NORMAL, (int32_t) scifTaskData.i2cAndAdcSensors.output.adcValue);
        scOutputData.batteryValue = AUXADCValueToMicrovolts(AUXADC_FIXED_REF_VOLTAGE_NORMAL, (int32_t) scifTaskData.i2cAndAdcSensors.output.batteryValue);

        // Acknowledge the alert event
        scifAckAlertEvents();

        PIN_setInterrupt(interruptAssctHandle, Board_ASSCT|PIN_IRQ_NEGEDGE);
        Timer_start(Timer0);
    }

} // taskReadFxn

void taskSendFxn(UArg a0, UArg a1) {

	UART_Params uartParams;
	UART_Handle uart;

	/* Create a UART with data processing off. */
		UART_Params_init(&uartParams);
		uartParams.writeDataMode = UART_DATA_BINARY;
		uartParams.readEcho = UART_ECHO_OFF;
		uartParams.baudRate = 9600;  //Above 9600 on MCU 0xFF added on the end of transmission

	while(1) {
		Semaphore_pend(Semaphore_handle(&semAssct), BIOS_WAIT_FOREVER);
		Timer_stop(Timer0);
		j=0;
		uart = UART_open(Board_UART, &uartParams);

			if (uart == NULL) {
				System_abort("Error opening the UART");
			}
		 PIN_setOutputValue(xbeePinHandle, Board_VTRANS_EN, 1);
		 UART_write(uart, &scOutputData, sizeof(scOutputData));
		 UART_close(uart);
		 PIN_setOutputValue(xbeePinHandle, Board_VTRANS_EN, 0);
		 PIN_setInterrupt(interruptSleepHandle, Board_SLEEP_IND|PIN_IRQ_NEGEDGE);
		 PIN_setOutputValue(xbeePinHandle, Board_XBEE_SLP, 1);
	}
} // taskSendFxn

void sleepCallbackFxn(PIN_Handle handle, PIN_Id pinId) {
	PIN_setInterrupt(interruptSleepHandle, Board_SLEEP_IND|PIN_IRQ_DIS);
	PIN_setOutputValue(xbeePinHandle, Board_3V3_EN, 0);
	PIN_setOutputValue(xbeePinHandle, Board_XBEE_SLP, 0);
}

void assctCallbackFxn(PIN_Handle handle, PIN_Id pinId) {
	i++;
	if(i==1) {
	PIN_setInterrupt(interruptAssctHandle, Board_ASSCT|PIN_IRQ_DIS);
	i=0;
	Semaphore_post(Semaphore_handle(&semAssct));
	}
}

/*
 *  ======== main ========
 */
int main(void) {
    Task_Params taskReadParams, taskSendParams;

    // Initialize drivers
    Board_initGeneral();
    UART_init();

    // Configure the OS tasks
    Task_Params_init(&taskSendParams);
	taskSendParams.stack = sendTaskStack;
	taskSendParams.stackSize = sizeof(sendTaskStack);
	taskSendParams.priority = 2;
	Task_construct(&sendTask, taskSendFxn, &taskSendParams, NULL);

    Task_Params_init(&taskReadParams);
    taskReadParams.stack = readTaskStack;
    taskReadParams.stackSize = sizeof(readTaskStack);
    taskReadParams.priority = 1;
    Task_construct(&readTask, taskReadFxn, &taskReadParams, NULL);



    // Create the semaphore used to wait for Sensor Controller ALERT events
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&semScTaskAlert, 0, &semParams);
    Semaphore_construct(&semAssct, 0, &semParams);


    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
	if(!ledPinHandle) {
		System_abort("Error initializing pins\n");
	}

    xbeePinHandle = PIN_open(&xbeePinState, xbeePinTable);
	if(!xbeePinHandle) {
		System_abort("Error initializing pins\n");
	}

	interruptSleepHandle = PIN_open(&interruptSleepState, interruptSleepTable);
	if(!interruptSleepHandle) {
		System_abort("Error initializing pins\n");
	}

	interruptAssctHandle = PIN_open(&interruptAssctState, interruptAssctTable);
	if(!interruptAssctHandle) {
		System_abort("Error initializing pins\n");
	}

    /* Setup callback for SLEEP_IND pin */
	if (PIN_registerIntCb(interruptSleepHandle, &sleepCallbackFxn) != 0) {
		System_abort("Error registering button callback function");
	}

	/* Setup callback for ASSCT pin */
	if (PIN_registerIntCb(interruptAssctHandle, &assctCallbackFxn) != 0) {
		System_abort("Error registering button callback function");
	}

	scOutputData.deviceID = *((uint32_t *)(FCFG1_BASE + FCFG1_O_MAC_15_4_0));
    // Start TI-RTOS
    BIOS_start();
    return 0;

} // main

void connTimeExp(void) {

	j++;
	if (j==79) {
		Timer_stop(Timer0);
		j=0;
		PIN_setInterrupt(interruptAssctHandle, Board_ASSCT|PIN_IRQ_DIS);
		PIN_setInterrupt(interruptSleepHandle, Board_SLEEP_IND|PIN_IRQ_DIS);
		PIN_setOutputValue(xbeePinHandle, Board_VTRANS_EN, 0);
		PIN_setOutputValue(xbeePinHandle, Board_3V3_EN, 0);
		PIN_setOutputValue(xbeePinHandle, Board_XBEE_SLP, 0);

	}


}


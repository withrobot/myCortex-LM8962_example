//*****************************************************************************
//
// adc.c - Example main source file
//
// Copyright (c) 2003-2010 Withrobot, Inc.  All rights reserved.
//
// Software License Agreement
//
// Withrobot, Inc.(Withrobot) is supplying this software for use solely and
// exclusively on Withrobot's products.
//
// The software is owned by Withrobot and/or its suppliers, and is protected
// under applicable copyright laws.  All rights are reserved.
// Any use in violation of the foregoing restrictions may subject the user
// to criminal sanctions under applicable laws, as well as to civil liability
// for the breach of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// WITHROBOT SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
// OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <string.h>

#include "hw_types.h"
#include "hw_memmap.h"

#include "gpio.h"
#include "adc.h"
#include "uart.h"
#include "timer.h"

#include "ustdlib.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define BUFFER_LEN      32

static void UARTSend(const char *pucBuffer, unsigned long ulCount);

int main(void)
{
    unsigned long adc_result[16];
    unsigned long cnt;
    char buffer[BUFFER_LEN];

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // Configure UART to transmit ADC results
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    //
    // Configure ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

    ADCSequenceDisable(ADC_BASE, 0);
    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_TIMER, 0);

    ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC_BASE, 0);

    //
    // Configure Timer to trigger ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure( TIMER0_BASE, TIMER_CFG_32_BIT_PER );
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    TimerLoadSet( TIMER0_BASE, TIMER_A, SysCtlClockGet() / 2 );     // 2Hz trigger
    TimerEnable( TIMER0_BASE, TIMER_A );

    //
    // Loop forever.
    //
    while(1)
    {
        while(!ADCIntStatus(ADC_BASE, 0, false));
        ADCIntClear(ADC_BASE, 0);
        cnt = ADCSequenceDataGet(ADC_BASE, 0, adc_result);
        if (cnt == 1)
        {
            usnprintf(buffer, BUFFER_LEN, "%d\r\n", adc_result[0]);
            buffer[BUFFER_LEN - 1] = 0;
            UARTSend(buffer, strlen(buffer));
        }
    }
}

static void UARTSend(const char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART0_BASE, *pucBuffer++);
    }
}


//*****************************************************************************
//
// adc_2ch.c - Example main source file
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

#include <stdio.h>
#include <string.h>

#include "hw_types.h"
#include "hw_memmap.h"

#include "sysctl.h"
#include "gpio.h"
#include "adc.h"
#include "timer.h"

#include "llio.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif



static void ADCIntHandler(void);

int main(void)
{
    unsigned long adc_result[16];
    unsigned long cnt;
    float temperature;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);


    //
    // Configure low-level I/O to use printf()
    //
    llio_init(115200);


    //
    // Configure ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

    // ADC sequence #0 - setup with 2 conversions
    ADCSequenceDisable(ADC_BASE, 0);
    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCIntRegister(ADC_BASE, 0, ADCIntHandler);
    ADCIntEnable(ADC_BASE, 0);

    // sequence step 0 - channel 0
    ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0);
    // sequence step 1 - internal temperature sensor. Generate Interrupt & End of Sequence
    ADCSequenceStepConfigure(ADC_BASE, 0, 1, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC_BASE, 0);

    //
    // Configure Timer to trigger ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure( TIMER0_BASE, TIMER_CFG_32_BIT_PER );
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
    TimerLoadSet( TIMER0_BASE, TIMER_A, SysCtlClockGet() / 2 );     // 2Hz trigger
    TimerEnable( TIMER0_BASE, TIMER_A );


    printf("\r\n\r\nADC 2 Channel Example\r\n");

    //
    // Loop forever.
    //
    while(1)
    {
        cnt = ADCSequenceDataGet(ADC_BASE, 0, adc_result);
        if (cnt == 2)
        {
            // Calculate temperature
            temperature = (2.7 - (float)adc_result[1] * 3.0 / 1024.) * 75. - 55.;

            printf("%d,%d,%.1f\r\n", adc_result[0], adc_result[1], temperature);
        }
        SysCtlSleep();
    }
}

static void ADCIntHandler(void)
{
    ADCIntClear(ADC_BASE, 0);
}

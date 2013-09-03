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

#define BUFFER_LEN      32


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


    printf("\r\n\r\nADC Temperature Sensor Example\r\n");

    //
    // Loop forever.
    //
    while(1)
    {
        while(!ADCIntStatus(ADC_BASE, 0, false));
        ADCIntClear(ADC_BASE, 0);
        cnt = ADCSequenceDataGet(ADC_BASE, 0, adc_result);      // Read ADC result
        if (cnt == 1)
        {
            // Calculate temperature
            temperature = (2.7 - (float)adc_result[0] * 3.0 / 1024.) * 75. - 55.;

            printf("%d, %.1f[degC]\r\n", adc_result[0], temperature);
        }
    }
}


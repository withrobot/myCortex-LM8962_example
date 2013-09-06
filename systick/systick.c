//*****************************************************************************
//
// systick.c - SysTick timer example main source file
//
// Copyright (c) 2003-2013 Withrobot, Inc.  All rights reserved.
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

#include "hw_types.h"
#include "hw_memmap.h"

#include "sysctl.h"
#include "systick.h"

#include "led.h"



#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

static void SysTickIntHandler(void);

static volatile unsigned long tick_count;

int main(void)
{
    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);



    //
    // Set up GPIO for LED
    //
    LED_INIT();



    //
    // Set up the period for the SysTick timer.
    //
    SysTickPeriodSet(SysCtlClockGet() / 1000);   // 1kHz SysTick timer


    //
    // Enable the SysTick Interrupt.
    //
    SysTickIntEnable();
    SysTickIntRegister(SysTickIntHandler);

    //
    // Enable SysTick.
    //
    SysTickEnable();


    //
    // Loop forever.
    //
    while(1)
    {
        SysCtlSleep();              // sleep here till interrupt

        if (tick_count % 1000 == 0) // toggle LED for every seconds.
            LED_TOGGLE();
    }
}


void SysTickIntHandler(void)
{
    tick_count++;               // increment counter value for every 1ms.
}


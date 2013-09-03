//*****************************************************************************
//
// timer.c - Timer example main source file
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

/*
 * Revision History
 *
 * when         who         what
 * ------------ ----------- ---------------------------------------------------
 * 2008-Jul-25  irmus       Use 'led.h'
 */

#include "hw_types.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "gpio.h"
#include "timer.h"
#include "interrupt.h"

#include "led.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

static void TimerIntHandler(void);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // Configure Timer to toggle LED
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure( TIMER0_BASE, TIMER_CFG_32_BIT_PER );
    TimerLoadSet( TIMER0_BASE, TIMER_A, SysCtlClockGet() / 2 );     // 2Hz timer -> 1Hz LED blinking
    TimerIntRegister( TIMER0_BASE, TIMER_A, TimerIntHandler );
    IntMasterEnable();
    TimerIntEnable( TIMER0_BASE, TIMER_TIMA_TIMEOUT );
    TimerEnable( TIMER0_BASE, TIMER_A );

    //
    // Configure GPIO to drive LED
    //
    LED_INIT();

    //
    // Loop forever.
    //
    while(1)
    {
    }
}

static void TimerIntHandler(void)
{
    // Clear interrupt flag
    TimerIntClear( TIMER0_BASE, TIMER_TIMA_TIMEOUT );

    if (LED_IS_ON())
        LED_OFF();
    else
        LED_ON();

}


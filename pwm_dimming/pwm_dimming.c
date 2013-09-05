//*****************************************************************************
//
// pwm.c - PWM generation example main source file
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
#include "gpio.h"
#include "pwm.h"
#include "systick.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

static void SysTickIntHandler(void);
static unsigned long duty;
static void SetPWMDuty(unsigned long duty);


int main(void)
{
    unsigned long period;

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);




    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);



    //
    // Set GPIO F0 as PWM pin.  They are used to output the PWM0
    //
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);


    //
    // Compute the PWM period based on the system clock.
    //
    period = SysCtlClockGet() / 440;


    // Set the PWM period to 440 (A) Hz.
    //
    PWMGenConfigure(PWM_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, period);

    //
    // Set PWM0 to a duty cycle of 25% and PWM1 to a duty cycle of 75%.
    //
    SetPWMDuty(25);

    //
    // Enable the PWM0 and PWM1 output signals.
    //
    PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);

    //
    // Enable the PWM generator.
    //
    PWMGenEnable(PWM_BASE, PWM_GEN_0);

    //
    // Set up the period for the SysTick timer.
    //
    SysTickPeriodSet(SysCtlClockGet() / 100);   // 100Hz SysTick timer


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
    }
}


void SysTickIntHandler(void)
{
    if (++duty >= 100)
        duty = 0;

    SetPWMDuty(duty);
}



static void SetPWMDuty(unsigned long duty)
{
    unsigned long period = SysCtlClockGet() / 440;

    PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, period * duty / 100);
}





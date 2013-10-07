//*****************************************************************************
//
// pwm_ccp_dimming.c - LED dimming example with CCP(PWM), ADC
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

#include "hw_types.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "timer.h"
#include "gpio.h"
#include "adc.h"

#define PWM_FREQUENCY 25000
#define PWM_MAX_DUTY 1000


static void VR_Init();
static void ADCIntHandler(void);

static void PWM_Init(void);
static void PWM_SetDuty(unsigned short duty);
static void PWM_Enable(void);
static void PWM_Disable(void);


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

int main(void)
{
    //
    // Set the clocking to run directly from the crystal.
    //
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    //
    // Initialize Timer CCP module for PWM
    //
    PWM_Init();
    PWM_Disable();

    PWM_SetDuty(750);   // 25% duty
    PWM_Enable();

    VR_Init();


    //
    // Loop forever.
    //
    while(1)
    {
        SysCtlSleep();
    }
}

static void VR_Init()
{
    //
    // Configure ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

    // ADC sequence #0 - setup with 1 conversions
    ADCSequenceDisable(ADC_BASE, 0);
    ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCIntRegister(ADC_BASE, 0, ADCIntHandler);
    ADCIntEnable(ADC_BASE, 0);

    // sequence step 0 - channel 0
    ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC_BASE, 0);

    //
    // Configure Timer to trigger ADC
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure( TIMER1_BASE, TIMER_CFG_32_BIT_PER );
    TimerControlTrigger(TIMER1_BASE, TIMER_A, true);
    TimerLoadSet( TIMER1_BASE, TIMER_A, SysCtlClockGet() / 100 );     // 1kHz control loop
    TimerEnable( TIMER1_BASE, TIMER_A );

}


static void ADCIntHandler(void)
{
    long cnt;
    unsigned long adc_result[16];
    unsigned short duty;

    ADCIntClear(ADC_BASE, 0);

    cnt = ADCSequenceDataGet(ADC_BASE, 0, adc_result);  // read ADC result
    if (cnt == 1)
    {
        duty = adc_result[0] * PWM_MAX_DUTY / 1023;        // calculate duty ratio

        PWM_SetDuty(duty);                              // Set PWM duty
    }
}

/*
 * Configure Timer0-A for PWM generation.
 * Timer0-A is connected to PD4/CCP0 pin.
 */
static void PWM_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_4);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PWM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / PWM_FREQUENCY);

    PWM_SetDuty(PWM_MAX_DUTY / 2);
}

/*
 * Sets PWM output duty cycle.
 * PWM duty cycle is allowed between 0~PWM_MAX_DUTY
 */
static void PWM_SetDuty(unsigned short duty)
{
    unsigned long d;

    if (duty > PWM_MAX_DUTY)
        duty = PWM_MAX_DUTY;
    else if (duty == 0)
        duty = 1;

    d = SysCtlClockGet() / PWM_FREQUENCY * (PWM_MAX_DUTY - duty) / PWM_MAX_DUTY;

    TimerMatchSet(TIMER0_BASE, TIMER_A, d);
}

static void PWM_Enable(void)
{
    TimerEnable(TIMER0_BASE, TIMER_A);
}

static void PWM_Disable(void)
{
    TimerDisable(TIMER0_BASE, TIMER_A);
}

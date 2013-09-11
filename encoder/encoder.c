//*****************************************************************************
//
// encoder.c - Quadrature Encoder Interface example main source file
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

#include <stdio.h>

#include "hw_types.h"
#include "hw_memmap.h"

#include "sysctl.h"
#include "systick.h"
#include "qei.h"

#include "led.h"
#include "llio.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

static void SysTickIntHandler(void);
void SW_IntHandler(void);



int main(void)
{
    long dir;
    unsigned long vel, pos;



    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);



    //
    // Set up low level I/O for printf()
    //
    llio_init(115200);


    //
    // Set up GPIO for LED
    //
    LED_INIT();



    //
    // Set up QEI peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_6);
    QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_NO_RESET |
                QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 0xffffffff);
    QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, SysCtlClockGet() / 10);

    QEIEnable(QEI0_BASE);
    QEIVelocityEnable(QEI0_BASE);



    //
    // Set up GPIO for encoder push switch
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTC_BASE, SW_IntHandler);
    GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_5);




    //
    // Set up the period for the SysTick timer.
    //
    SysTickPeriodSet(SysCtlClockGet() / 10);   // 10Hz SysTick timer


    //
    // Enable the SysTick Interrupt.
    //
    SysTickIntEnable();
    SysTickIntRegister(SysTickIntHandler);

    //
    // Enable SysTick.
    //
    SysTickEnable();

    printf("\r\nEncoder example\r\n");


    //
    // Loop forever.
    //
    while(1)
    {
        SysCtlSleep();              // sleep here till interrupt

        LED_TOGGLE();

        dir = QEIDirectionGet(QEI0_BASE);
        vel = QEIVelocityGet(QEI0_BASE);
        pos = QEIPositionGet(QEI0_BASE);
        printf("%d,%d,%d\r\n", dir, vel, pos);
    }
}


void SysTickIntHandler(void)
{

}

void SW_IntHandler(void)
{
    GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_5);
    printf("Switch PUSH\r\n");

    QEIPositionSet(QEI0_BASE, 0);

}





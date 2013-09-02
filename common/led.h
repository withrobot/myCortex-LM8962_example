//*****************************************************************************
//
// led.h - Defines and functions for LED in myCortex-LM8962 board.
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
// This is part of myCortex-LMXXXX series examples.
//*****************************************************************************

#ifndef LED_H_
#define LED_H_

#ifndef PART_LM3S8962
#error This header file is designed for myCortex-LM8962 board only.
#endif

#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "interrupt.h"


#define CUST_SYSCTL_PERIPH_LED          SYSCTL_PERIPH_GPIOG
#define CUST_GPIO_PORT_BASE             GPIO_PORTG_BASE
#define CUST_GPIO_PIN                   GPIO_PIN_0

#define LED_INIT()      do {                                                            \
                            SysCtlPeripheralEnable(CUST_SYSCTL_PERIPH_LED);             \
                            GPIOPinTypeGPIOOutput(CUST_GPIO_PORT_BASE, CUST_GPIO_PIN);  \
                        } while(0)

#define LED_ON()        GPIOPinWrite(CUST_GPIO_PORT_BASE, CUST_GPIO_PIN, 0)
#define LED_OFF()       GPIOPinWrite(CUST_GPIO_PORT_BASE, CUST_GPIO_PIN, CUST_GPIO_PIN)

#define LED_IS_ON()     (GPIOPinRead(CUST_GPIO_PORT_BASE, CUST_GPIO_PIN) != CUST_GPIO_PIN)

#define LED_TOGGLE()                                                    \
        do {                                                            \
            if (LED_IS_ON())                                            \
                LED_OFF();                                              \
            else                                                        \
                LED_ON();                                               \
        } while(0)

#define LED_DEBUG_BLINK()                       \
    do {                                        \
        unsigned long time;                     \
        time = SysCtlClockGet() / 6;            \
        IntMasterDisable();                     \
        while(1) {                              \
            LED_ON();                           \
            SysCtlDelay(time);                  \
            LED_OFF();                          \
            SysCtlDelay(time);                  \
        }                                       \
    } while(0)
#endif /*LED_H_*/

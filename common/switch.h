//*****************************************************************************
//
// switch.h - Defines and functions for push switch.
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

/*
 * 이 파일은 myCortex-LM8962보드의 PC7핀에 push switch를 연결한 경우를 위해
 * 만들어져 있다. 참고로 PC7핀은 boot loader에서 사용하는 핀이다.
 */
#ifndef SWITCH_H_
#define SWITCH_H_

#ifndef PART_LM3S8962
#error This header file is designed for myCortex-LM8962 board only.
#endif

#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "hw_memmap.h"

#define CUST_SYSCTL_PERIPH_SW           SYSCTL_PERIPH_GPIOC
#define CUST_SW_PORT_BASE               GPIO_PORTC_BASE
#define CUST_SW_PIN                     GPIO_PIN_7
#define CUST_SW_INT_EDGE                GPIO_FALLING_EDGE

// input을 사용하는 경우 초기화.
#define SW_INIT()                                                           \
    do {                                                                    \
        SysCtlPeripheralEnable(CUST_SYSCTL_PERIPH_SW);                      \
        GPIOPinTypeGPIOInput(CUST_SW_PORT_BASE, CUST_SW_PIN);               \
        GPIOPadConfigSet(CUST_SW_PORT_BASE, CUST_SW_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); \
    } while(0)

// interrupt input을 사용하는 경우 초기화.
// 이 경우에는 SW_IntHandler() 함수를 구현해야 한다.
#define SW_INIT_INT()                                                       \
    do {                                                                    \
        SysCtlPeripheralEnable(CUST_SYSCTL_PERIPH_SW);                      \
        GPIOPinTypeGPIOInput(CUST_SW_PORT_BASE, CUST_SW_PIN);               \
        GPIOPadConfigSet(CUST_SW_PORT_BASE, CUST_SW_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); \
        GPIOIntTypeSet(CUST_SW_PORT_BASE, CUST_SW_PIN, CUST_SW_INT_EDGE);   \
        GPIOPortIntRegister(CUST_SW_PORT_BASE, SW_IntHandler);              \
    } while(0)

#define SW_IS_HIGH()        (GPIOPinRead(CUST_SW_PORT_BASE, CUST_SW_PIN) == CUST_SW_PIN)
#define SW_IS_PUSHED()      (!SW_IS_HIGH())
#define SW_INT_ENABLE()     GPIOPinIntEnable(CUST_SW_PORT_BASE, CUST_SW_PIN)
#define SW_INT_DISABLE()    GPIOPinIntDisable(CUST_SW_PORT_BASE, CUST_SW_PIN)
#define SW_INT_CLEAR()      GPIOPinIntClear(CUST_SW_PORT_BASE, CUST_SW_PIN)
void SW_IntHandler(void);

#endif /*SWITCH_H_*/

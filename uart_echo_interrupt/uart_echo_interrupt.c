//*****************************************************************************
//
// uart_echo_interrupt.c - Example main source file
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
#include "hw_ints.h"

#include "sysctl.h"
#include "gpio.h"
#include "uart.h"
#include "led.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


static void UART_IntHandler(void);


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);


    //
    // Configure UART
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // FIFO 사이즈 = 16바이트(TX, RX 각각)
    // TX fifo는 4바이트 이하 사용중일때,
    // RX fifo는 12바이트 이상 사용중일때 인터럽트 발생.
    // 즉 TX는 쌓아두고 보낼 데이타가 4개밖에 남지 않았음을 알려주고
    // RX는 새로 들어오는 데이타를 쌓아둘 공간이 4개 밖에 남지 않았음을 알려준다.
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX2_8, UART_FIFO_RX6_8);


    // UART 수신 인터럽트만 활성화시킨다.
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntRegister(UART0_BASE, UART_IntHandler);
    UARTEnable(UART0_BASE);

    IntEnable(INT_UART0);


    //
    // Configure GPIO to drive LED
    //
    LED_INIT();



    //
    // Loop forever.
    //
    while(1)
    {
        if (UARTCharsAvail(UART0_BASE))
        {
            LED_TOGGLE();
            UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
        }
    }
}

static void UART_IntHandler(void)
{
    unsigned long ulStatus;

    IntDisable(INT_UART0);
    ulStatus = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ulStatus);

    if (((ulStatus & UART_INT_RX) == UART_INT_RX) || ((ulStatus & UART_INT_RT) == UART_INT_RT))
    {
        while(UARTCharsAvail(UART0_BASE))
        {
            LED_TOGGLE();
            UARTCharPutNonBlocking(UART0_BASE, UARTCharGet(UART0_BASE));
        }
    }

    IntEnable(INT_UART0);
}



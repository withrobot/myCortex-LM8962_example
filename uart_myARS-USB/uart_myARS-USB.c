//*****************************************************************************
//
// uart_myARS-USB.c - ARS UART interface example main source file
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
#include "uart.h"

#include "led.h"
#include "llio.h"


#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif



int main(void)
{
    int acc_x_raw, acc_y_raw, acc_z_raw, gyro_x_raw, gyro_y_raw, temp_raw;
    int roll_raw, pitch_raw;
    char *ptr;
    char buff[80];

    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y;
    float temp;
    float roll, pitch;


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
    // Configure UART1 for myARS-USB
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    UARTEnable(UART1_BASE);


    printf("\r\nmyARS-USB UART interface example\r\n");


    //
    // Loop forever.
    //
    while(1)
    {
        // Read characters from myARS-USB while new line character is received.
        for(ptr = buff; ; ptr++)
        {
            *ptr = UARTCharGet(UART1_BASE);     // blocking read

            if (*ptr == '\n')
            {
                *ptr = 0;       // null termination
                break;
            }
        }

        if (buff[0] == '$')
        {
            sscanf(buff+1, "%d,%d,%d,%d,%d,%d,%d,%d", &acc_x_raw, &acc_y_raw, &acc_z_raw, &gyro_x_raw, &gyro_y_raw, &temp_raw, &roll_raw, &pitch_raw);

            acc_x = (float)acc_x_raw * 9.8 / 2048.;
            acc_y = (float)acc_y_raw * 9.8 / 2048.;
            acc_z = (float)acc_z_raw * 9.8 / 2048.;

            gyro_x = (float)gyro_x_raw * 0.232;
            gyro_y = (float)gyro_y_raw * 0.232;

            temp = (float)temp_raw * 0.1;

            roll = (float)roll_raw * 0.01;
            pitch = (float)pitch_raw * 0.01;

            printf("ACC[m/s/s]:%.2f,%.2f,%.2f\r\n", acc_x, acc_y, acc_z);
            printf("GYRO[deg/s]:%.2f,%.2f\r\n", gyro_x, gyro_y);
            printf("TEMP[degC]:%.1f\r\n", temp);
            printf("R/P[deg]:%.2f,%.2f\r\n\r\n", roll, pitch);
            LED_TOGGLE();
        }
    }
}


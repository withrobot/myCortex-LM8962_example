//*****************************************************************************
//
// printf_scanf.c - Example main source file
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
#include <yfuns.h>


#include "hw_types.h"
#include "hw_memmap.h"

#include "sysctl.h"
#include "uart.h"

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
    int n;
    char buff[128];

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);


    //
    // Configure low-level I/O to use printf() and scanf()
    //
    llio_init(115200);



    printf("\r\n\r\nLow-level I/O Example\r\n");
    printf("Enter any number...\r\n");


    //
    // Loop forever.
    //
    while(1)
    {
        if (scanf("%d", &n) == 1)
        {
            printf("%d is entered.\r\n", n);

            printf("\r\nEnter any number...\r\n");
        }
        else
            scanf("%s", buff);      // flush read FIFO
    }
}


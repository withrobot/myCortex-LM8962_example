//*****************************************************************************
//
// i2c_Serial_EEPROM.c - I2C interface Serial EEPROM example main source file
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
#include "hw_i2c.h"

#include "sysctl.h"
#include "i2c.h"

#include "led.h"
#include "llio.h"


#define EEPROM_SLAVE_ADDRESS        0x50        // 0b1010000



unsigned long I2C_Write(unsigned short addr, unsigned char *buff, unsigned long len);
unsigned long I2C_Read(unsigned short addr, unsigned char *buff, unsigned long len);





#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif



int main(void)
{
    unsigned char buff[4];



    //
    // Set the clocking to use PLL
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
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
    // Configure I2C0 for myARS-USB
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), true);  // 400 kHz



    printf("\r\n24LC64 serial EEPROM I2C interface example\r\n");


#if 1
    buff[0] = 0x45;
    buff[1] = 0x19;
    buff[2] = 0xdd;
    buff[3] = 0xfa;
    I2C_Write(0, buff, 4);
    printf("Write to address 0...\r\n");
    printf("%02X %02X %02X %02X\r\n", buff[0], buff[1], buff[2], buff[3]);
#else
    I2C_Read(0, buff, 4);
    printf("Read from address 0...\r\n");
    printf("%02X %02X %02X %02X\r\n", buff[0], buff[1], buff[2], buff[3]);
#endif

    //
    // Loop forever.
    //
    while(1)
    {
    }
}


unsigned long I2C_Write(unsigned short addr, unsigned char *buff, unsigned long len)
{
    unsigned long l = len;

    I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, EEPROM_SLAVE_ADDRESS, false);
    I2CMasterDataPut(I2C0_MASTER_BASE, (unsigned char )(addr >> 8));
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_START | I2C_MCS_RUN);
    while(I2CMasterBusy(I2C0_MASTER_BASE));

    I2CMasterDataPut(I2C0_MASTER_BASE, (unsigned char)addr);
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_RUN);
    while(I2CMasterBusy(I2C0_MASTER_BASE));

    while(l--)
    {
        I2CMasterDataPut(I2C0_MASTER_BASE, *buff++);
        if (l == 0)
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_STOP | I2C_MCS_RUN);
        else
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_RUN);
        while(I2CMasterBusy(I2C0_MASTER_BASE));
    }

    return len;
}

unsigned long I2C_Read(unsigned short addr, unsigned char *buff, unsigned long len)
{
    unsigned long l;

    if (len == 0)
        return 0;

    if (!buff)
        return 0;

    I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, EEPROM_SLAVE_ADDRESS, false);
    I2CMasterDataPut(I2C0_MASTER_BASE, (unsigned char)(addr >> 8));
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_START | I2C_MCS_RUN);
    while(I2CMasterBusy(I2C0_MASTER_BASE));

    I2CMasterDataPut(I2C0_MASTER_BASE, (unsigned char)addr);
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_RUN);
    while(I2CMasterBusy(I2C0_MASTER_BASE));

    if (len == 1)
    {
        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, EEPROM_SLAVE_ADDRESS, true);
        I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_START | I2C_MCS_STOP | I2C_MCS_RUN);
        while(I2CMasterBusy(I2C0_MASTER_BASE));
        *buff = I2CMasterDataGet(I2C0_MASTER_BASE);
    }
    else
    {
        l = len;
        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, EEPROM_SLAVE_ADDRESS, true);
        I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_ACK | I2C_MCS_START | I2C_MCS_RUN);
        while(I2CMasterBusy(I2C0_MASTER_BASE));
        *buff++ = I2CMasterDataGet(I2C0_MASTER_BASE);

        while(l--)
        {
            if (l == 0)
                I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_STOP | I2C_MCS_RUN);
            else
                I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_ACK | I2C_MCS_RUN);
            while(I2CMasterBusy(I2C0_MASTER_BASE));
            *buff++ = I2CMasterDataGet(I2C0_MASTER_BASE);
        }
    }

    return len;
}



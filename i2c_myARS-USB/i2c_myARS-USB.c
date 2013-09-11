//*****************************************************************************
//
// i2c_myARS-USB.c - ARS I2C interface example main source file
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
#include "systick.h"

#include "led.h"
#include "llio.h"
#include "myARS-USB.h"





void SysTickIntHandler(void);
unsigned long I2C_Write(unsigned char addr, unsigned char *buff, unsigned long len);
unsigned long I2C_Read(unsigned char addr, unsigned char *buff, unsigned long len);





#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


typedef struct
{
    short acc_x;
    short acc_y;
    short acc_z;
    short gyro_x;
    short gyro_y;
    short temp;
    short roll;
    short pitch;
} ars_t;

int main(void)
{
    ars_t ars;

    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y;
    float temp;
    float roll, pitch;

    unsigned char who_am_i;


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
    // Configure I2C0 for myARS-USB
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    I2CMasterInitExpClk(I2C0_MASTER_BASE, SysCtlClockGet(), true);  // 400 kHz


    //
    // Set up the period for the SysTick timer.
    //
    SysTickPeriodSet(SysCtlClockGet() / 100);   // 100Hz SysTick timer
    SysTickIntEnable();
    SysTickIntRegister(SysTickIntHandler);
    SysTickEnable();



    printf("\r\nmyARS-USB I2C interface example\r\n");

    I2C_Read(ARS_REG_WHO_AM_I, &who_am_i, 1);
    printf("who am i = 0x%02X\r\n", who_am_i);


    //
    // Loop forever.
    //
    while(1)
    {
        SysCtlSleep();


        // read registers from myARS-USB via I2C interface
        I2C_Read(ARS_REG_ACC_X_LOW, (unsigned char *)&ars, sizeof(ars_t));


        acc_x = (float)ars.acc_x * 9.8 / 2048.;
        acc_y = (float)ars.acc_y * 9.8 / 2048.;
        acc_z = (float)ars.acc_z * 9.8 / 2048.;

        gyro_x = (float)ars.gyro_x * 0.232;
        gyro_y = (float)ars.gyro_y * 0.232;

        temp = (float)ars.temp * 0.1;

        roll = (float)ars.roll * 0.01;
        pitch = (float)ars.pitch * 0.01;

        printf("ACC[m/s/s]:%.2f,%.2f,%.2f\r\n", acc_x, acc_y, acc_z);
        printf("GYRO[deg/s]:%.2f,%.2f\r\n", gyro_x, gyro_y);
        printf("TEMP[degC]:%.1f\r\n", temp);
        printf("R/P[deg]:%.2f,%.2f\r\n\r\n", roll, pitch);
        LED_TOGGLE();
    }
}

void SysTickIntHandler(void)
{
}


unsigned long I2C_Write(unsigned char addr, unsigned char *buff, unsigned long len)
{
    unsigned long l = len;

    I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, ARS_SLAVE_ADDRESS, false);
    I2CMasterDataPut(I2C0_MASTER_BASE, addr);
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_START | I2C_MCS_RUN);
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

unsigned long I2C_Read(unsigned char addr, unsigned char *buff, unsigned long len)
{
    unsigned long l;

    if (len == 0)
        return 0;

    if (!buff)
        return 0;

    I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, ARS_SLAVE_ADDRESS, false);
    I2CMasterDataPut(I2C0_MASTER_BASE, addr);
    I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_START | I2C_MCS_RUN);
    while(I2CMasterBusy(I2C0_MASTER_BASE));

    if (len == 1)
    {
        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, ARS_SLAVE_ADDRESS, true);
        I2CMasterControl(I2C0_MASTER_BASE, I2C_MCS_STOP | I2C_MCS_START | I2C_MCS_RUN);
        while(I2CMasterBusy(I2C0_MASTER_BASE));
        *buff = I2CMasterDataGet(I2C0_MASTER_BASE);

    }
    else
    {
        l = len;
        I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, ARS_SLAVE_ADDRESS, true);
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



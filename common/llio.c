/*
 * Low-level I/O
 * Standard input/output support
 */

#include <stdio.h>
#include <stddef.h>
#include <yfuns.h>

#include "hw_types.h"
#include "hw_memmap.h"

#include "gpio.h"
#include "uart.h"



void llio_init(unsigned long baudrate)
{
    //
    // Configure UART
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), baudrate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
}


#pragma module_name = "?__write"
static int MyLowLevelPutchar(int ch);
static int MyLowLevelGetchar();

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */

  size_t nChars = 0;

  if (buffer == 0)
  {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }

  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size)
  {
    if (MyLowLevelPutchar(*buffer++) < 0)
    {
      return _LLIO_ERROR;
    }

    ++nChars;
  }

  return nChars;
}

static int MyLowLevelPutchar(int ch)
{

    UARTCharPut(UART0_BASE, ch);

    return 1;
}


size_t __read(int handle, unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */

  int nChars = 0;

  /* This template only reads from "standard in", for all other file
   * handles it returns failure. */
  if (handle != _LLIO_STDIN)
  {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size > 0; --size)
  {
    int c = MyLowLevelGetchar();
    if (c < 0)
      break;

    *buffer++ = c;
    ++nChars;
  }

  return nChars;
}

static int MyLowLevelGetchar()
{
    return (int)UARTCharGet(UART0_BASE);
}


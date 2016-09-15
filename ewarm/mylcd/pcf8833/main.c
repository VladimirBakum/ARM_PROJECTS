//---------------------------------------------------------------
// ���������������� ������ ��� Stellaris Launchpad LM4F120
// ��� IAR ARM 6.4
// ������ ��������� �� ������������� ���������� StellarisWare
// By (R)soft 5 Jan 2014 version 1.0
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_12MHZ -> 88 MHz Clock -> UART0 Work OK
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_10MHZ -> 109 MHz Clock -> UART0 Work OK

#define UART_BUFFERED

#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "pcf8833/pcf8833.h"
#include "pcf8833/pcf8833.c"

#define RED_LED   GPIO_PIN_1 // ���� ����� ��� 0x02
#define BLUE_LED  GPIO_PIN_2 // ���� ����� ��� 0x04
#define GREEN_LED GPIO_PIN_3



int main(void)
{


  // ��������� ������������ 80 ��� �� PLL � ������� �����������.
  // ������ ������������ ������� �������� ������� ������� 200 ���
  // �� �������� SYSCTL_SYSDIV_x (x=2...64)
//  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_10MHZ);
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_24MHZ);

  // ��������� UART0 @115200bps N81.
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);
  ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
  ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);

  //Clear the terminal
  UARTprintf("\033[2J");
  // Show the clock frequency on the display.
  UARTprintf("Stellaris Cortex-M4F board @ %u MHz\n", ROM_SysCtlClockGet() / 1000000);
  UARTprintf("Flash size %u kB\n", ROM_SysCtlFlashSizeGet() / 1024);
  UARTprintf("SRAM size %u kB\n", ROM_SysCtlSRAMSizeGet() / 1024);
  UARTprintf("PWM Clock %u Hz\n", ROM_SysCtlPWMClockGet() );
  UARTprintf("ADC Speed %u Hz\n", ROM_SysCtlADCSpeedGet() );
  UARTprintf("\nTest UART0\n");
  UARTprintf("-----------------------------------\n");

  
  // ��������� � ������������ ����� F GPIO ��� ������ � LED
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);
//  GPIOPadConfigSet(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // ������ ���������� ������������, ������ ������ �������
  // ������ �������� - ����� ��� ������
  // ������ �������� - ����� ���� ����������
  ROM_GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);

  // ��������� ��� ����������
  ROM_GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, 0);

  // ��������. ���� ������� 8 ��� �� �������� � �������� �����
  // SysCtlDelay(8000000/6);
//  ROM_SysCtlDelay(2000000);

  pcf8833_init();    
  pcf8833_cls(BLACK); 

//  srand(0x2345);
  
  pcf8833_pixel(50,60,RED);
  pcf8833_putstr("Nokia 6100",10,10,LARGE,BLUE,YELLOW);
  pcf8833_putstr("LCD",58,26,LARGE,PINK,GREEN);
  pcf8833_putstr("Colour",10,26,MEDIUM,RED,ORANGE);  
  pcf8833_putstr("Temperature",5,46,LARGE,WHITE,BLACK);  
  pcf8833_putstr("PCF8833 CONTROLLER",2,114,SMALL,WHITE,BLACK);  
  pcf8833_draw(120,10,5,105,GREEN);

  pcf8833_setrect(5,64,91,95,1,BLUE);
  pcf8833_setrect(4,63,92,96,0,WHITE);

  
  
  while (1) 
  {
    

    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, GREEN_LED);

    unsigned char x;
    for (x=0;x<140;x++)
    {
      pcf8833_putchar32('+',x+5,64,RED,BLUE);
      pcf8833_putchar32('1',x+25,64,WHITE,BLUE);
      pcf8833_putchar32('5',x+45,64,WHITE,BLUE);
      pcf8833_putchar32('.',x+65,64,WHITE,BLUE);    
      pcf8833_putchar32('4',x+72,64,WHITE,BLUE);
    }
//    delay_ms(10); 
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, 0);
    
  }

}

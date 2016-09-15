//---------------------------------------------------------------
// Демонстрационный проект для Stellaris Launchpad LM4F120
// для IAR ARM 6.4
// Проект ссылается на установленную библиотеку StellarisWare
// By (R)soft 5 Jan 2014 version 1.0
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_12MHZ -> 88 MHz Clock -> UART0 Work OK
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_10MHZ -> 109 MHz Clock -> UART0 Work OK

#define UART_BUFFERED

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "pcf8814/pcf8814.h"
#include "pcf8814/pcf8814.c"

#define RED_LED   GPIO_PIN_1 // тоже самое что 0x02
#define BLUE_LED  GPIO_PIN_2 // тоже самое что 0x04
#define GREEN_LED GPIO_PIN_3



int main(void)
{


  // Установка тактирования 80 МГц от PLL с внешним резонатором.
  // Расчет тактирования ведется делением базовой частоты 200 Мгц
  // на делитель SYSCTL_SYSDIV_x
//  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_10MHZ);
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

  // Настройка UART0 @115200bps N81.
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

  
  // Включение и конфигурация порта F GPIO для работы с LED
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);
//  GPIOPadConfigSet(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // Пример управления светодиодами, зажечь только красный
  // Второй параметр - маска для сброса
  // Третий параметр - какие биты установить
  ROM_GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);

  // Выключить все светодиоды
  ROM_GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, 0);

  // Задержка. Если частота 8 МГц то задержка в секундах равна
  // SysCtlDelay(8000000/6);
//  ROM_SysCtlDelay(2000000);

  pcf8814_init();
//  pcf8814_putchar(0x41);
//  pcf8814_puts("Hello!");
//  usprintf(LN2,"ID1=%02X",id1);
  usprintf(LN1,"Texas Cortex-M4F");
  usprintf(LN2,"Board @ %u MHz", ROM_SysCtlClockGet() / 1000000);
  usprintf(LN3,"Real Overclock");
  usprintf(LN4,"is 110 MHz! :o)");
  usprintf(LN5,"Flash size %ukB", ROM_SysCtlFlashSizeGet() / 1024);
  usprintf(LN6,"SRAM size %ukB", ROM_SysCtlSRAMSizeGet() / 1024);
  usprintf(LN7,"Nokia 1200 LCD");
  usprintf(LN8,"By (R)soft 2014");


  pcf8814_update_console();

  while (1) 
  {
    

    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, GREEN_LED);
//    delay_us(1);
    usprintf(LN7,"Nokia 1200 LCD");
    pcf8814_update_console();
    
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, 0);
//    delay_us(5);

    usprintf(LN7,"SyqoG 9012 ZYK");
    pcf8814_update_console();
    
    
    //ROM_SysCtlDelay( (ROM_SysCtlClockGet()/(3*1000))*ms ) ;  // more accurate
    //SysCtlDelay( (SysCtlClockGet()/(3*1000))*ms ) ;  // less accurate    
    
  }

}

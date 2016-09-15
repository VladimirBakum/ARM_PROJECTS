//---------------------------------------------------------------
// Демонстрационный проект для Stellaris Launchpad LM4F120
// для IAR ARM 6.4
// Проект ссылается на установленную библиотеку StellarisWare
// By (R)soft 5 Jan 2014 version 1.0
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_12MHZ -> 88 MHz Clock -> UART0 Work OK
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_10MHZ -> 109 MHz Clock -> UART0 Work OK

#define UART_BUFFERED

#include <string.h> // for memcpy
//#include "inc/lm4f120h5qr.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
//#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "pcf8814/pcf8814h.h"
#include "pcf8814/pcf8814h.c"

#include "n93/n93.h"
#include "n93/n93.c"

/*
#include "n93\tjpgd.h"
#include "n93\integer.h"
#include "n93\tjpgd.c"
#include "n93\colors24.h"
#include "n93\jpeg.c"
*/

#define KEY1            GPIO_PIN_0
#define RED_LED         GPIO_PIN_1 // тоже самое что 0x02
#define BLUE_LED        GPIO_PIN_2 // тоже самое что 0x04
#define GREEN_LED       GPIO_PIN_3
#define KEY2            GPIO_PIN_4

const unsigned int LEDS_ALL = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;

const unsigned int SW1 = GPIO_PIN_0;
const unsigned int SW2 = GPIO_PIN_4;

//-----------------------------------------------------
// Пример разблокирования ноги PF0
// Список выводов со специальным назначением после Reset:
// PA[1:0] UART0 GPIOPCTL=0x01
// PA[5:2] SSI0  GPIOPCTL=0x02
// PB[3:2] I2C0  GPIOPCTL=0x03
// PC[3:0] JTAG/SWD GPIOCTL=0x01
// PD7 NMI
// PF0 NMI
void unlock_PF0 (void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  HWREG(GPIO_PORTF_BASE | GPIO_O_LOCK) = GPIO_LOCK_KEY_DD; // Разблокировка (именно DD!!!)
  HWREG(GPIO_PORTF_BASE | GPIO_O_CR) |= GPIO_PIN_0; // Позволяет писать в регистры AFSEL,PUR,PDR и DEN
                                                     // Соотв. выбранной ноги
  HWREG(GPIO_PORTF_BASE | GPIO_O_AFSEL) &= 0xFE; // Если стоит 1 то конфиг по умолчанию (после сброса)
                                                // если 0 - переконфигур.(Alternate Function Select)
  HWREG(GPIO_PORTF_BASE | GPIO_O_DEN) &= 0xFE; // Digital Enable
  HWREG(GPIO_PORTF_BASE | GPIO_O_PCTL) = 0x00; // Port Control - устанавливает весь порт F как GPIO
  
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00; // Закрытие блокировки
  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}

void crSet(unsigned int base, unsigned char value)
{
    unsigned long v = (HWREG(GPIO_PORTF_BASE + GPIO_O_CR) & 0xFFFFFF00) | value;
    HWREG(base + GPIO_O_CR) = v;
}

void myproc (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH2_GPIOF);
    SysCtlDelay(2);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LEDS_ALL);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, SW1 | SW2);

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    crSet(GPIO_PORTF_BASE, 1);
    GPIOPadConfigSet(GPIO_PORTF_BASE, SW1 | SW2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    crSet(GPIO_PORTF_BASE, 0);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}


int main(void)
{

  // Установка тактирования 80 МГц от PLL с внешним резонатором.
  // Расчет тактирования ведется делением базовой частоты 200 Мгц
  // на делитель SYSCTL_SYSDIV_x
//  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_12_2MHZ);
//  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_13_5MHZ); // 25 MHz OSC
  SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);


  // Включение и конфигурация порта F GPIO для работы с LED
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  unlock_PF0();
//  GPIOPinConfigure(GPIO_PF0_U1RTS);
  
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTF_AHB_BASE, KEY1 | RED_LED |BLUE_LED | GREEN_LED | KEY2 );
  GPIOPadConfigSet(GPIO_PORTF_AHB_BASE, KEY1 | GREEN_LED | RED_LED | BLUE_LED | KEY2, \
                   GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // Port A
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOA); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTA_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTA_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // Port E
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE); // Advanced BUS Enable
  GPIOPinTypeGPIOOutput(GPIO_PORTE_AHB_BASE, 0xFF );
  GPIOPadConfigSet(GPIO_PORTE_AHB_BASE, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
//  GPIO_PORTD_DIR_R = 0xFF;
//  GPIO_PORTD_DEN_R = 0xFF;

  
  pcf8814_init();
  N93_init();
//  N93_rainbow16();

  usprintf(LN1,"Texas Cortex-M4F");
  usprintf(LN2,"Board @ %u MHz", ROM_SysCtlClockGet() / 1000000);
  usprintf(LN3,"Real Overclock");
  usprintf(LN4,"is 110 MHz! :o)");
  usprintf(LN5,"Flash size %ukB", ROM_SysCtlFlashSizeGet() / 1024);
  usprintf(LN6,"SRAM size %ukB", ROM_SysCtlSRAMSizeGet() / 1024);
  usprintf(LN7,"Nokia 1200 LCD");
  usprintf(LN8,"By (R)soft 2014");

  
  pcf8814_update_console();


//  N93_rainbow16();
/*
  N93_putpixel16(10,10,WHITE_COLOR);
  N93_draw(200,10,20,300,GREEN_COLOR);
  N93_draw(210,10,30,300,BLUE_COLOR);
  N93_setrect(100,50,200,150,1,GREEN_COLOR);
  N93_setrect(200,250,100,200,0,WHITE_COLOR);
  N93_circle(100,150,85,RED_COLOR);

  N93_putstr("LCD",158,46,2,PINK_COLOR,CYAN_COLOR);
  N93_putstr("Colour",110,46,1,RED_COLOR,ORANGE_COLOR);
  N93_putstr("Temperature",55,86,2,BLUE_COLOR,TRANSPARENT);
  N93_putstr("MC2PA8201 CONTROLLER",2,124,0,WHITE_COLOR,TRANSPARENT);
*/
  
  DrawScreen();

/*
  unsigned int i;
  for (i=0;i<320;i=i+10)
  {
    N93_draw(0,320,240,i,RED_COLOR);
  }
  for (i=0;i<320;i=i+10)
  {
    N93_draw(240,0,0,i,YELLOW_COLOR);
  }
*/
  
  
/*  
//----------- JPEG DECODING ----------------------  1
  JDEC jd;		// TJDEC decompression object
  IODEV iodev;	// Identifier of the decompression session (depends on application)
  JRESULT rc;		// Result
  // jpeg output    
  iodev.jpic = COLORS24;
  iodev.jsize = sizeof(COLORS24);
  iodev.joffset = 0;

  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);
*/  
  
  
  while (1) 
  {
/*
    unsigned int i;
    for (i=0;i<210;i++)
    {
        N93_putstr("LCD",i,276,2,WHITE_COLOR,BLACK_COLOR);
        delay_ms(20);
    }
*/
    
//    HWREG(GPIO_PORTF_AHB_BASE | (0xFF << 2)) = 0xFF;
//    HWREG(GPIO_PORTA_AHB_BASE | (0xFF << 2)) = 0xFF;
//    HWREG(GPIO_PORTE_AHB_BASE | (0xFF << 2)) = 0xFF;    
    delay_us(1);
    HWREG(GPIO_PORTF_AHB_BASE | (0xFF << 2)) = 0;
    HWREG(GPIO_PORTA_AHB_BASE | (0xFF << 2)) = 0;
    HWREG(GPIO_PORTE_AHB_BASE | (0xFF << 2)) = 0;    
    delay_us(1);
/*    
    unsigned int state = ~GPIOPinRead(GPIO_PORTF_BASE, SW1 | SW2);
    led = ((state & SW1) << 1) | ((state & SW2) >> 1);
    GPIOPinWrite(GPIO_PORTF_BASE, LEDS_ALL, led);
    */
  }

}

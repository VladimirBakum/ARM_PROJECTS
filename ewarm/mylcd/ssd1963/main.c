//---------------------------------------------------------------
// ƒемонстрационный проект дл€ Stellaris Launchpad LM4F120
// дл€ IAR ARM 6.4
// ѕроект ссылаетс€ на установленную библиотеку StellarisWare
// By (R)soft 6 March 2015 revision 1.0 special for SSD1963


#include <string.h> // for memcpy

//#include "inc/lm4f120h5qr.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include "utils/uartstdio.h"
#include "utils/ustdlib.h" // for usprintf

#include "system.c" // –азличные вспомогательные процедуры

#include "ssd1963/ssd1963_8bit.h"
#include "ssd1963/ssd1963_8bit.c"


//==================================
//==========  M A I N  =============
//==================================

int main(void)
{

  system_init();


  // For debug -> Enable uart
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioInit(0);
  UARTprintf("\033[2JDebug output Ready!\n");

  
  //
  // Configure SysTick for a 100Hz interrupt.  The FatFs driver wants a 10 ms
  // tick.
  //
  SysTickPeriodSet(SysCtlClockGet() / 100);
  SysTickEnable();
  SysTickIntEnable();

  //
  // Enable Interrupts
  //
  IntMasterEnable();
  
  ssd1963_init();
    
  ssd1963_rainbow();
  delay_ms(400);
  ssd1963_clear(BLACK_COLOR);

  ssd1963_putpixel(0,0,WHITE_COLOR);
  ssd1963_setrect(100,50,200,150,1,GREEN_COLOR);
  ssd1963_setrect(200,250,100,200,0,WHITE_COLOR);

  ssd1963_setrect(0,2,271,476,0,WHITE_COLOR); // с тачем не видно 2 верхних и 3 нижних строки

  ssd1963_draw(200,10,20,240,YELLOW_COLOR);
  ssd1963_draw(210,10,30,240,BLUE_COLOR);
  ssd1963_circle(100,150,85,RED_COLOR);

  ssd1963_setpos(172,46);
  ssd1963_setcolor(PINK_COLOR, CYAN_COLOR);
  fontsize=BIG_FONT;
  ssd1963_putstr("TFT");

  ssd1963_setpos(172,62);
  bColor= TRANSPARENT;
  ssd1963_putstr("LCD 272x480");
  
  ssd1963_setpos(110,46);
  ssd1963_setcolor(RED_COLOR, ORANGE_COLOR);
  fontsize=MEDIUM_FONT;
  ssd1963_putstr("Colour");
  
  ssd1963_setpos(55,86);
  ssd1963_setcolor(BLUE_COLOR,TRANSPARENT);
  fontsize=BIG_FONT;
  ssd1963_putstr("Temperature");

  ssd1963_setpos(102,135);
  fColor = RED_COLOR;
  ssd1963_putstr("Transparent");

  ssd1963_setpos(110,155);
  fColor = CYAN_COLOR;
  ssd1963_putstr("Background");

  ssd1963_setpos(12,24);
  fColor=WHITE_COLOR;
  fontsize=SMALL_FONT;
  ssd1963_putstr("SSD1963 LCD TFT CONTROLLER");

  y=260;
  x=0;
  fontsize = MEDIUM_FONT;
  ssd1963_printf("Texas LM4F120H5QR Cortex-M4F\n");
  ssd1963_printf("Board @ %u MHz\n", ROM_SysCtlClockGet() / 1000000);
  ssd1963_printf("Flash size: %u kB\n", ROM_SysCtlFlashSizeGet() / 1024);
  ssd1963_printf("SRAM size: %u kB x:%u y:%u fnt:%x\n", (ROM_SysCtlSRAMSizeGet() / 1024),x,y, fontsize);
  ssd1963_printf("ITDB02-4.3 LCD\nBy (R)soft 2015");

  fontsize = BIG_FONT;
  fColor = RED_COLOR;
  ssd1963_printf("\nmicroSD with Fat32 support.\nJPEG & BMP & & PNG & ZX screens\nfiles output.\n");
  fColor = GREEN_COLOR;  
  ssd1963_printf("Touchscreen x2046 support.\n\n");
  fColor = YELLOW_COLOR;
  ssd1963_printf("Touch screen to Continue...");

  
  while (1) 
  {
    
  }

}



//---------------------------------------------------------------
// Демонстрационный проект для Stellaris Launchpad LM4F120
// для IAR ARM 6.4
// Проект ссылается на установленную библиотеку StellarisWare
// By (R)soft 5 Jan 2014 version 1.0
// Version 1.1 26-04-2014 with JPG & BMP out from microSD & touchscreen with TSC2046

#define MICROSD
#define JPEGENABLE
#define BMPENABLE
#define PNGENABLE
#define SPECCY
#define TOUCH

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

#include "system.c" // Различные вспомогательные процедуры

#include "ili9481/ili9481_8bit.h"
#include "ili9481/ili9481_8bit.c"

#ifdef MICROSD
#include "fatfs/ff.h"   // for fat system
#include "fatfs/diskio.h"
#include "fatfs/ff.c"
#include "fatfs/mmc-ek-lm4f232h5qd.c"
#include "fatfs/sd_card.c"
#endif //MICROSD

#ifdef JPEGENABLE
#include "ili9481/jpeg/tjpgd.h"
#include "ili9481/jpeg/integer.h"
#include "ili9481/jpeg/tjpgd.c"
#include "ili9481/jpeg/jpeg.c"
#endif //JPEGENABLE

#ifdef BMPENABLE
#include "ili9481/bmp/bmp.h"
#include "ili9481/bmp/bmp.c"
#endif //BMPENABLE

#ifdef TOUCH
#include "tsc2046/tsc2046.c"
#endif //TOUCH

#ifdef SPECCY
#include "ili9481/speccy/speccy.c"
#endif //SPECCY

#ifdef PNGENABLE
#include "ili9481/png/lodepng.c"
#endif // PNGENABLE

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
  
  ili9481_init();
  
#ifdef TOUCH  
  touch_init(); // Инициализация Touch
#endif
  
  ili9481_rainbow();
  delay_ms(400);
  ili9481_clear(BLACK_COLOR);

//  N93_rainbow_zx();

  ili9481_putpixel(0,0,WHITE_COLOR);
  ili9481_setrect(100,50,200,150,1,GREEN_COLOR);
  ili9481_setrect(200,250,100,200,0,WHITE_COLOR);

  ili9481_setrect(0,2,271,476,0,WHITE_COLOR); // с тачем не видно 2 верхних и 3 нижних строки

  ili9481_draw(200,10,20,240,YELLOW_COLOR);
  ili9481_draw(210,10,30,240,BLUE_COLOR);
  ili9481_circle(100,150,85,RED_COLOR);

  ili9481_setpos(172,46);
  ili9481_setcolor(PINK_COLOR, CYAN_COLOR);
  fontsize=BIG_FONT;
  ili9481_putstr("TFT");

  ili9481_setpos(172,62);
  bColor= TRANSPARENT;
  ili9481_putstr("LCD 272x480");
  
  ili9481_setpos(110,46);
  ili9481_setcolor(RED_COLOR, ORANGE_COLOR);
  fontsize=MEDIUM_FONT;
  ili9481_putstr("Colour");
  
  ili9481_setpos(55,86);
  ili9481_setcolor(BLUE_COLOR,TRANSPARENT);
  fontsize=BIG_FONT;
  ili9481_putstr("Temperature");

  ili9481_setpos(102,135);
  fColor = RED_COLOR;
  ili9481_putstr("Transparent");

  ili9481_setpos(110,155);
  fColor = CYAN_COLOR;
  ili9481_putstr("Background");

  ili9481_setpos(12,24);
  fColor=WHITE_COLOR;
  fontsize=SMALL_FONT;
  ili9481_putstr("ILI9481 LCD TFT CONTROLLER");

  y=260;
  x=0;
  fontsize = MEDIUM_FONT;
  ili9481_printf("Texas LM4F120H5QR Cortex-M4F\n");
  ili9481_printf("Board @ %u MHz\n", ROM_SysCtlClockGet() / 1000000);
  ili9481_printf("Flash size: %u kB\n", ROM_SysCtlFlashSizeGet() / 1024);
  ili9481_printf("SRAM size: %u kB x:%u y:%u fnt:%x\n", (ROM_SysCtlSRAMSizeGet() / 1024),x,y, fontsize);
  ili9481_printf("TFT8K2346 LCD\nBy (R)soft 2014");

  fontsize = BIG_FONT;
  fColor = RED_COLOR;
  ili9481_printf("\nmicroSD with Fat32 support.\nJPEG & BMP & & PNG & ZX screens\nfiles output.\n");
  fColor = GREEN_COLOR;  
  ili9481_printf("Touchscreen TSC2046 support.\n\n");
  fColor = YELLOW_COLOR;
  ili9481_printf("Touch screen to Continue...");


/*
#ifdef TOUCH
  wait_touch();
#else
  wait_key();
#endif
*/
  
#ifdef MICROSD
  ili9481_setpos(0,2);
  ili9481_setcolor(CYAN_COLOR, BLACK_COLOR);
  fontsize=SMALL_FONT;

  UARTprintf("Init SD ");
  while(disk_initialize(0)); // Висим, пока карточка проинициализируется
  UARTprintf("OK\n");
  
  UARTprintf("Mounting FS ");
  unsigned char rc = f_mount(&fso, "0", 1); // Mount FAT with drive num and mount immediately
  UARTprintf("%s\n", rc);

//  ili9481_printf("Opening %s\n",PATH); //chahge dir
//  f_chdir("/MODS");

/*
  out_jpg("Jolie3.jpg");
//  wait_key();
  
  out_jpg("Jolie2.jpg");
  wait_key();
  
  out_jpg("Jolie1.jpg");
  wait_key();
*/




//  out_png(105, 60, "P_24B.PNG"); // NORMAL.PNG RIGHT.PNG LEFT.PNG NEXT.PNG

  
  //  wait_touch();


//  out_jpg(20, 64, "Jolie1.jpg");


#ifdef TOUCH
  wait_touch();
#else
  wait_key();
#endif



//  out_jpg("Heard0.jpg");

  ili9481_setcolor(WHITE_COLOR,BLACK_COLOR);  
//    file_list();  
  
  show_files("."); // показывает файлы

#endif //MICROSD



//  ili9481_setcolor(WHITE_COLOR,BLACK_COLOR);
//  fontsize=BIG_FONT;

  
  while (1) 
  {


    
#ifdef TOUCH
      touch_setrect();
#endif
    
  }

}



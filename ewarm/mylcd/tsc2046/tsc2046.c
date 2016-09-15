//-------------------------------------------
// TSC2046 (ADS7846) Driver
// Version 1.1 by (R)soft 2014
// In v1.1 -> 3-wire SPI

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "integer.h"
#include <stdbool.h>

// Peripheral definitions for TSC2046

#define TSC_DELAY       200 // Delay value for 3-wire SPI (64=100kHz CLK)

// TSC port
#define TSC_BASE            GPIO_PORTB_AHB_BASE
#define TSC_SYSCTL_PERIPH   SYSCTL_PERIPH_GPIOB

// GPIO for SPI pins
#define TSC_CLK             GPIO_PIN_4
#define TSC_IO              GPIO_PIN_5 // Input/Output Pin
#define TSC_CSX             GPIO_PIN_6
#define TSC_IRQX            GPIO_PIN_7 // IRQX

// Определения вкл/выкл ног
#define TSC_CLK_1   HWREG(PORT_CTRL + (TSC_CLK << 2)) = TSC_CLK;
#define TSC_CLK_0   HWREG(PORT_CTRL + (TSC_CLK << 2)) = 0;
#define TSC_IO_1    HWREG(PORT_CTRL + (TSC_IO << 2)) = TSC_IO;
#define TSC_IO_0    HWREG(PORT_CTRL + (TSC_IO << 2)) = 0;
#define TSC_CSX_1   HWREG(PORT_CTRL + (TSC_CSX << 2)) = TSC_CSX;
#define TSC_CSX_0   HWREG(PORT_CTRL + (TSC_CSX << 2)) = 0;

#define TSC_INPUT   GPIODirModeSet(TSC_BASE, TSC_IO, GPIO_DIR_MODE_IN);
#define TSC_OUTPUT  GPIODirModeSet(TSC_BASE, TSC_IO, GPIO_DIR_MODE_OUT);


//-------------------------------------------------------------
// TSC2046 Commands:
// S A2 A1 A0 MOD S/D PD1 PD0 (PD1&PD0=0 - IRQ ENABLE) (MODE=0 12-bit)
// 1 1  0  1  1   0   0   0
#define TOUCH_X 0xD8

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 0  0  1  1   0   0   0
#define TOUCH_Y 0x98

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 0  1  1  1   0   0   0
#define TOUCH_Z1 0xB8

// S A2 A1 A0 MOD S/D PD1 PD0
// 1 1  0  0  1   0   0   0
#define TOUCH_Z2 0xC8

// 1 0 1 0 0 1 0 0
#define TOUCH_VBAT 0xA4

// 1 1 1 0 0 1 0 0
#define TOUCH_AUX 0xE4

// 1 0 0 0 0 1 0 0
#define TOUCH_TEMP0 0x84

// 1 1 1 1 0 1 0 0
#define TOUCH_TEMP1 0xF4




//=============================================================
// Чтение байта from Touch
// Сначала MSB (первым идет старший бит)

unsigned char touch_read (void)
{
  TSC_IO_1;
  TSC_INPUT; // Шина данных на ввод
  unsigned char i;
  unsigned char result=0;
  unsigned char mask=0x80;
  unsigned char data;

  for (i=0; i<8; i++)
  {
    TSC_CLK_1; // Чтение по нарастанию SCLK, данные выставились
    SysCtlDelay( TSC_DELAY );

    data = GPIOPinRead(TSC_BASE, TSC_IO);  // Чтение с шины IO
    if (data != 0)
      {
        result |= mask; // Если была единица то делаем OR результата и маски
      }
    mask>>=1;   // Двигаем маску

    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );
    
  }
  
  TSC_OUTPUT;
  
  return (result);
}


//===========================================
// Отправка байта to Touch

void touch_write(unsigned char byte)
{
    if (byte&0x80) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );
    
    if (byte&0x40) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    

    if (byte&0x20) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    
    
    if (byte&0x10) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    
    
    if (byte&0x08) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    
    
    if (byte&0x04) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    
    
    if (byte&0x02) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    
    
    if (byte&0x01) TSC_IO_1
    else           TSC_IO_0;
    TSC_CLK_1;
    SysCtlDelay( TSC_DELAY );
    TSC_CLK_0;
    SysCtlDelay( TSC_DELAY );    

}


//--------------------------------------------------
// Инициализация Touch

static
void touch_init (void)
{

  SysCtlPeripheralEnable(TSC_SYSCTL_PERIPH);
  SysCtlGPIOAHBEnable(TSC_SYSCTL_PERIPH); // Включить режим Advanced
  GPIOPinTypeGPIOOutput( TSC_BASE, (TSC_CLK | TSC_IO | TSC_CSX) );
  GPIOPadConfigSet( TSC_BASE, (TSC_CLK | TSC_IO | TSC_CSX), \
                   GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  TSC_CSX_1; // Исходно CSX=1
  TSC_CLK_0;
  TSC_IO_1;
}

//--------------------------------------------------------------
// Отправка команды в TSC2046 и получение 8-битного резульатата 

unsigned char touch_getbyte (unsigned char command)  //communication with TSC2046
{
  unsigned char data;
  
  TSC_CSX_0; // Выбор кристалла
  SysCtlDelay( TSC_DELAY );
  
  touch_write(command);  // send command
  
  data = touch_read();  // Читаем старший байт
  touch_read();         // читаем младший байт и его игнор

  TSC_CSX_1;
  SysCtlDelay( TSC_DELAY );
  
  return data;
}

//----------------------------------------------------------
/*
Так как у меня было всего одно устройство на шине, то CS был закорочен на 0. 
А читать данные я начинал по сигналу PENIRQ. 
Вначале читал как ты в цикле X,Y,X,Y… на матрице в 3 дюйма вроде неплохо, 
но в один прекрасный день принесли матрицу в 15 дюймов, 
вот тут и начались проблемы, данные начали скакать, 
пальцем нажал — одни показания, стайлусом — другие. 
И тут я решил залезть на сайт TI и поискать у них решение. 
И ура, я его нашел…
Сделал все как у них написано:
1 — При запуске посылаем команду 0xD8
2 — Потом по сигналу PENIRQ начинаю считывать координату X три раза 
командами 0xD9 0xD9 0xD8, затем
3 — Потом по сигналу PENIRQ начинаю считывать координату Y три раза 
командами 0x99 0x99 0x98
4 — Одно значение по X и одно значение по Y будет неправильным, 
а по два оставшихся будут практически идентичными.

После того, как переделал все по этому алгоритму, значения стали стабильными, 
перестали плавать от силы нажатия и метода нажатия ( стайлус или палец ).

Так испробуй по сигналу CS подать команду 0xD8, а затем пункты 2-4
*/


/*
//--------------------------------------------
// Ожидание прерывания от Touch
void  touch_irq(void)
{
  unsigned char tmp;
  while(1)
  {
  HWREG(TSC_GPIO_PORT_BASE | (TSC_IRQX << 2)) = TSC_IRQX; // TSC_IRQX=1

  GPIOPinTypeGPIOInput(TSC_GPIO_PORT_BASE, TSC_IRQX); // TSC_IRQX на ввод
  tmp = GPIOPinRead(TSC_GPIO_PORT_BASE, TSC_IRQX); // Read TSC_IRQX

  if (tmp==0) break;
  
  GPIOPinTypeGPIOOutput(TSC_GPIO_PORT_BASE, TSC_IRQX); // TSC_IRQX на вывод
  GPIOPadConfigSet(TSC_GPIO_PORT_BASE, TSC_IRQX, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  };

  UARTprintf("Touch IRQ! ");
}
*/

//-------------------------------------------------------
// Значения координат прямоугольника в области нажатия
unsigned short x1_rect, x2_rect, y1_rect, y2_rect;


//-------------------------------------------------
// Опрос тача один раз и если было нажатие, то 
// вычисляем координаты прямоугольника и значения нажатия
// Return: 0 - not touched, value - сила нажатия.
// Т.к. контроллер тача TSC2046 находится на одной шине с microSD,
// а карточка работает на частоте 12.5 МГц, то перед работой с SPI
// нужно понизить скорость SPI до 400 кГц. А затем, после работы
// процедуры, восстановить скорость.

unsigned char touch_poll (void)
{
  unsigned char x1, x2, x3, y2, y3;
  unsigned char z1;

  //Делаем несколько пустых выборок для улучшения результата помехоустойчивости
//  touch_getbyte(TOUCH_X); // dummy read 
//  touch_getbyte(TOUCH_Y); // dummy read
  
  x1 = touch_getbyte(TOUCH_X); //f0 e8
  x2 = touch_getbyte(TOUCH_X); //e8 e0
  x3 = touch_getbyte(TOUCH_X); //e0 d8

  touch_getbyte(TOUCH_Y); // dummy read
  y2 = touch_getbyte(TOUCH_Y);
  y3 = touch_getbyte(TOUCH_Y);
  
//  touch_getbyte(TOUCH_X); // dummy read
//  touch_getbyte(TOUCH_Y); // dummy read
  
  z1 = touch_getbyte(TOUCH_Z1);
//  z2 = touch_getbyte(TOUCH_Z2);
    
  // Если необходимые значения совпали, определяем интервал вывода прямоугольника
  if ((x1==x2 && x2==x3) && (y2==y3))
    {

      unsigned short interval_x = ((ILI9481_PIXELX) / (13+1));
      unsigned short interval_y = ((ILI9481_PIXELY+4) / (11+1));
      
      switch (x1) // берем x1 как наиболее точную координату
      {
      case 0x88:
          x1_rect = 0; x2_rect = interval_x; break;
      case 0x90:
          x1_rect = interval_x; x2_rect = interval_x * 2; break;
      case 0x98:
          x1_rect = interval_x * 2; x2_rect = interval_x * 3; break;
      case 0xA0:
          x1_rect = interval_x * 3; x2_rect = interval_x * 4; break;
      case 0xA8:
          x1_rect = interval_x * 4; x2_rect = interval_x * 5; break;
      case 0xB0:
          x1_rect = interval_x * 5; x2_rect = interval_x * 6; break;
      case 0xB8:
          x1_rect = interval_x * 6; x2_rect=interval_x * 7; break;
      case 0xC0:
          x1_rect = interval_x * 7; x2_rect=interval_x * 8; break;
      case 0xC8:
          x1_rect = interval_x * 8; x2_rect=interval_x * 9; break;
      case 0xD0:
          x1_rect = interval_x * 9; x2_rect=interval_x * 10; break;
      case 0xD8:
          x1_rect = interval_x * 10; x2_rect=interval_x*11; break;
      case 0xE0:
          x1_rect = interval_x*11; x2_rect=interval_x*12; break;
      case 0xE8:
          x1_rect = interval_x*12; x2_rect=interval_x*13; break;
      case 0xF0:
          x1_rect = interval_x*13; x2_rect=interval_x*14; break;
      default: break;
          
      }

      switch (y2) // Берем y2 как наиболее точную координату
      {
      case 0x90:
        y1_rect=0; y2_rect=interval_y; break;
      case 0x98:
        y1_rect = interval_y; y2_rect = interval_y * 2; break;
      case 0xA0:
        y1_rect = interval_y * 2; y2_rect = interval_y * 3; break;
      case 0xA8:
        y1_rect = interval_y * 3; y2_rect = interval_y * 4; break;
      case 0xB0:
        y1_rect = interval_y * 4; y2_rect = interval_y * 5; break;
      case 0xB8:
        y1_rect = interval_y * 5; y2_rect = interval_y * 6; break;
      case 0xC0:
        y1_rect = interval_y * 6; y2_rect = interval_y * 7; break;
      case 0xC8:
        y1_rect = interval_y * 7; y2_rect = interval_y * 8; break;
      case 0xD0:
        y1_rect = interval_y * 8; y2_rect = interval_y * 9; break;
      case 0xD8:
        y1_rect = interval_y * 9; y2_rect = interval_y * 10; break;
      case 0xE0:
        y1_rect = interval_y * 10; y2_rect = interval_y * 11; break;
      case 0xE8:
        y1_rect = interval_y * 11; y2_rect = interval_y * 12; break;
      default: break;
                
      }

//-----------------------------------------------------------
//      z_rect = ((x1_rect * z2) - z1) / z1; // Сила нажатия
//-----------------------------------------------------------

//------------------------------ for debug -------------------------------------      
//      ili9481_setpos(0,10);
//      ili9481_printf("X1:%x\nX2:%x\nX3:%x\nY2:%x\nY3:%x\nZ1:%x", x1, x2, x3, y2, y3, z1);
//------------------------------------------------------------------------------
      
      return z1; // Возвращаем силу нажатия
      
    }
  else
    {
//------------------------------ for rebug -------------------------------------
//      ili9481_setpos(0,180);
//      ili9481_printf("X1:%x\nX2:%x\nX3:%x\nY2:%x\nY3:%x", x1, x2, x3, y2, y3);
//------------------------------------------------------------------------------      
      return 0;

    }
}


//---------------------------------------
// Чтение картинки в буфер
// Параметры: координаты прямоугольника

void touch_rect_bufread (unsigned short x0, unsigned short y0, unsigned short x1, \
                          unsigned short y1)
{

  unsigned short i;
  unsigned short index = 0;
  
  ili9481_Window(x0, y0, x1, y1);
  ili9481_wr_cmd(ILI9481_MEMORY_READ);

  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_IN);  // Шина данных на ввод
  
  // По даташиту ILI9481 первые два байта чтения из памяти дисплея недействительны   
  RDX_0; // // Dummy Read First Byte
  RDX_1;

  RDX_0; // // Dummy Read Second Byte
  RDX_1;

  for (i = 0; i < ((x1 - x0 +1) * (y1 - y0 +1)); i++) //+2 for dummy bytes
    { 
      RDX_0;
      sdbufer[index++] = GPIOPinRead(PORT_DATA, 0xFF);  // Read byte
      RDX_1;

      RDX_0;
      sdbufer[index++] = GPIOPinRead(PORT_DATA, 0xFF);  // Read byte
      RDX_1;
    }
  
  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_OUT);  // Шина данных на вывод
  
  ili9481_wr_cmd(ILI9481_NOP);  // Break Memory Read

//  UARTprintf("RD: %02x%02x %02x%02x\n", sdbufer[2], sdbufer[3], sdbufer[4], sdbufer[5]);
}




//---------------------------------------
// Вывод картинки из буфера
// Параметры: координаты прямоугольника

void touch_rect_bufwrite (unsigned int x0, unsigned int y0, unsigned int x1, \
                          unsigned int y1)
{

  unsigned int i;
  unsigned int index = 0;
  
  ili9481_Window(x0, y0, x1, y1);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);
   
  for (i = 0; i < ((x1 - x0 +1) * (y1 - y0 +1)); i++) 
    { 
      ili9481_wr_dat(sdbufer[index++]); // Write HI byte
      ili9481_wr_dat(sdbufer[index++]); // Write LO byte
    } 

  ili9481_wr_cmd(ILI9481_NOP); // Break Write Memory

}


//-------------------------------------------------
// Опрос тача один раз и если было нажатие, то 
// выводим прямоугольник на некоторое время,
// затем возвращаем картинку на место.

void touch_setrect (void)
{
  unsigned char z;

  z = touch_poll();

  if (z) // Если было нажатие, то рисуем прямоугольник
  {
    //IntMasterDisable();

    // Запоминаем картинку в буфере
    touch_rect_bufread(y1_rect, x1_rect, y2_rect, x2_rect);
    
    // Выводим прямоугольник
    // Оси X и Y тачпанели и дисплея не совпадают, поэтому подставляем зеркально
    ili9481_setrect(y1_rect, x1_rect, y2_rect, x2_rect, 1, RED_COLOR);

    // Вывод значения координат в прямоугольнике
    ili9481_setcolor(WHITE_COLOR, RED_COLOR);
    fontsize = SMALL_FONT;
    
    ili9481_setpos(y1_rect, x1_rect);
    ili9481_printf("%d", y1_rect);
    ili9481_setpos(y1_rect, x1_rect+10);
    ili9481_printf("%d", x1_rect);
    ili9481_setpos(y1_rect, x1_rect+20);
    ili9481_printf("%d", z);
    
    delay_ms(100); // Некоторая задержка, чтобы увидеть прямоугольник    
    
    // Возвращаем картинку на место
    touch_rect_bufwrite(y1_rect, x1_rect, y2_rect, x2_rect);

    // Повторно выполняем ту же операцию чтобы картинка "выровнялась".
    // Почему нужно делать два раза - так и не понял.
    // Скорее всего при чтении данных, происходит XOR данных, поэтому
    // если сделать так два раза, то все восстанавливается.
    touch_rect_bufread(y1_rect, x1_rect, y2_rect, x2_rect);
    touch_rect_bufwrite(y1_rect, x1_rect, y2_rect, x2_rect);

    //IntMasterEnable();
    
  }
}


//-------------------------------------------------
// Опрос тача в цикле, пока не будет нажат

void wait_touch (void)
{

  while(!touch_poll()); // Крутимся в цикле пока не нажали

}



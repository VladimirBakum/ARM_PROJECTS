// MC2PA8201 Routine
// ********************************************************************
// Writen by (R)soft 12.11.2013 - 16.3.2014
// Version 1.0
// Testing on Stellaris LM4F120 Cortex-M4F MCU
// Testing on Nokia N93/N73/N71 LCD Module. Resolution 320x240 pixels. 
// Built-in MC2PA8201 controller with parallel 8-bit interface.
// Reference manual for controller - MC2PA8201.pdf
// ********************************************************************
// Прекрасно работает без delay
// CSX - всегда ноль

//#include <math.h> // for floor, trunc, round, ceil, etc..
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"

#include "fonts.h" // 3 Fonts
#include "scr.h" // ZX screens
//#include "jetpac.h" // NES nonstandart sprites
//#include "jjack.h"
//#include "nodesprt.h" // ZX Sprites
#include "nodes.h" // ZX Sprites from original game "Nodes of Yesod"
#include "arc.h" // ZX Sprites from Arc of Yesod
//#include "exolon.h" // ZX Sprites
#include "lisa.h"
#include "n93.h"


//=============================================================
// Запись команды в ЖКИ
void N93_write_command (unsigned char byte)
{
  DCX_0; // Режим команды
  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;
  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер
  DCX_1; // возвращаем основной режим - режим записи данных
}


//=============================================================
// Запись байта данных в ЖКИ

void N93_write_data (unsigned char byte)
{
  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;
  WRX_0; // WRX=0 - Write mode
  WRX_1; // байт передается в контроллер
}


//=============================================================
// Чтение байта из ЖКИ
// DCX при чтении всегда 1
// Эта функция писалась исключительно для отладки кода и
// отображения отклика MC2PA8201 - чтение регистров.

unsigned char N93_read_data(void)
{

  RDX_0;  // RDX=0 - Read mode  Строб чтения - данные выставились

  GPIOPinTypeGPIOInput(PORT_DATA, 0xFF); // Шина данных на ввод

  unsigned char data = GPIOPinRead(PORT_DATA, 0xFF);  // Чтение с шины данных
//  unsigned char data = HWREG(PORT_DATA + (GPIO_O_DATA + (0xFF << 2)));
    
  RDX_1;  // Возврат RDX в исходное состояние

  // Шина данных опять на вывод.
  // Вторую команду (GPIOPadConfigSet) нужно ставить, иначе не работает.
  GPIOPinTypeGPIOOutput(PORT_DATA, 0xFF);
  GPIOPadConfigSet(PORT_DATA, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);  
  
  return (data);
}



//--------------------------------------------
// Установка окна доступа в память дисплея

void N93_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

  if(x2>=N93_WIDTH) { x2 = N93_WIDTH-1; }
  if(y2>=N93_HEIGHT){ y2 = N93_HEIGHT-1; }

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x1>>8);
  N93_write_data(x1);
  N93_write_data(x2>>8);
  N93_write_data(x2);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y1>>8);
  N93_write_data(y1);
  N93_write_data(y2>>8);
  N93_write_data(y2);
}


//-----------------------------------------------------
// Очистка дисплея цветом 16-бит

void N93_Clear(unsigned int color)
{
  unsigned int x,y;
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1); // При задании окна важна ориентация ЖКИ
  N93_write_command(N93_MEMORY_WRITE);

  for (x = N93_PIXELX; x; x--) 
   {
       for (y = N93_PIXELY; y; y--) 
       { 
         N93_write_data((unsigned char)(color>>8));
         N93_write_data((unsigned char)(color));
       }
   }
  N93_write_command(N93_NOP);
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
    
}

//=============================================================
// Инициализация LCD и выводов порта данных/управления

void N93_init ()   
{        
  // Настройка портов LCD
  // ---------- Порт управления ----------------
  SysCtlPeripheralEnable(SYS_CTRL);
  SysCtlGPIOAHBEnable(SYS_CTRL); // Включить режим Advanced
  GPIOPinTypeGPIOOutput( PORT_CTRL, 
                            PIN_RESX | PIN_DCX | PIN_RDX | PIN_WRX );
  GPIOPadConfigSet( PORT_CTRL, 
                   PIN_RESX | PIN_DCX | PIN_RDX | PIN_WRX, 
                   GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // ------------- Порт данных ---------------------
  SysCtlPeripheralEnable(SYS_DATA);
  SysCtlGPIOAHBEnable(SYS_DATA); // Включить режим Advanced
  GPIOPinTypeGPIOOutput(PORT_DATA, 0xFF );
  GPIOPadConfigSet(PORT_DATA, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  RDX_1;  // Исходное состояние Read=1
  WRX_1;  // Исходное состояние Write=1
  DCX_1; // Исходно DCX всегда 1 (запись данных)
  
  RESX_0;  // Hardware Reset LCD
  delay_ms(2);
  RESX_1;  // Return Reset OFF
  delay_ms(150);

  N93_write_command(N93_SOFTWARE_RESET); // Soft Reset
  delay_ms(150);

  N93_write_command(N93_SLEEP_OUT);
  delay_ms(50);

  N93_write_command(N93_DISPLAY_INVERSION_OFF);
  N93_write_command(N93_IDLE_MODE_OFF);
  N93_write_command(N93_NORMAL_DISPLAY_MODE_ON);

  
  N93_write_command(N93_INTERFACE_PIXEL_FORMAT);
  N93_write_data(0x05); // 07 - 24bit Color, 05 - 16bit Color

  N93_write_command(N93_MEMORY_ACCESS_CONTROL);
  // D7         D6         D5              D4        D3       D2        D1  D0
  // направл Y, направл X, поменять X & Y, RefreshY, RGB/BGR, RefreshX, x,  x
  N93_write_data(LCD_ORIENTATION); 

  delay_ms(125);
  N93_write_command(N93_DISPLAY_ON);
  N93_write_command(N93_NOP);

  N93_Clear(BLACK);

}   

//-----------------------------------------
// Установка точки 24 bit/pixel
void N93_putpixel24(unsigned int x, unsigned int y, unsigned long color) 
{ 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data(x>>8);
  N93_write_data(x);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data(y>>8);
  N93_write_data(y);

  N93_write_command(N93_MEMORY_WRITE);

  N93_write_data((unsigned char)(color>>16));  // RED
  N93_write_data((unsigned char)(color>>8));  // GREEN
  N93_write_data((unsigned char)(color)); // BLUE

  N93_write_command(N93_NOP);
     
} 

//-----------------------------------------
// Установка точки 16 bit/pixel

void N93_putpixel16(unsigned int x, unsigned int y, unsigned int color) 
{ 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data(x>>8);
  N93_write_data(x);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data(y>>8);
  N93_write_data(y);

  N93_write_command(N93_MEMORY_WRITE);

  N93_write_data((unsigned char)(color>>8));
  N93_write_data((unsigned char)(color));

  N93_write_command(N93_NOP);
     
} 

//=======================================
// Заполнение n строк одним цветом 24 bit/pixel
void N93_fill24 (unsigned int lines, unsigned long color)
{    
 unsigned int a,b;   

// N93_write_command(N93_MEMORY_WRITE);

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(N93_WIDTH);b++)
         {
            N93_write_data((unsigned char)(color>>16));  // RED
            N93_write_data((unsigned char)(color>>8));  // GREEN
            N93_write_data((unsigned char)(color)); // BLUE
         }
   }
// N93_write_command(N93_NOP);
}

//=======================================
// Заполнение n строк одним цветом 16 bit/pixel
void N93_fill16 (unsigned int lines, unsigned int width, unsigned int color)
{    
 unsigned int a,b;   

// N93_write_command(N93_MEMORY_WRITE);

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(width);b++)
         {
            N93_write_data((unsigned char)(color>>8));
            N93_write_data((unsigned char)(color));
         }
   }
// N93_write_command(N93_NOP);
}

/*
//-----------------------------
// Вывод радуги  в 24-битном цвете
void N93_rainbow24 (void)
{
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill24(32,DEEPPINK);
  N93_fill24(32,BLUE);
  N93_fill24(32,PURPLE);  
  N93_fill24(32,BLUEVIOLET);    
  N93_fill24(32,YELLOW);    
  N93_fill24(32,RED);
  N93_fill24(32,GREEN);
  N93_fill24(32,BLACK);  
  N93_fill24(32,ORANGE);
  N93_fill24(32,CYAN);  
}
*/

//-----------------------------
// Вывод радуги  в 16-битном цвете
void N93_rainbow16 (void)
{
  N93_Window(0,0,N93_WIDTH-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill16(32,N93_WIDTH,OLIVE_COLOR);
  N93_fill16(32,N93_WIDTH,BLUE_COLOR);
  N93_fill16(32,N93_WIDTH,PURPLE_COLOR);  
  N93_fill16(32,N93_WIDTH,CYAN_COLOR);  
  N93_fill16(32,N93_WIDTH,YELLOW_COLOR);    
  N93_fill16(32,N93_WIDTH,RED_COLOR);
  N93_fill16(32,N93_WIDTH,GREEN_COLOR);
  N93_fill16(32,N93_WIDTH,BLACK_COLOR);  
  N93_fill16(32,N93_WIDTH,ORANGE_COLOR);
  N93_fill16(32,N93_WIDTH,MAGENTA_COLOR);    
}

//-----------------------------
// Вывод радуги  в 16-битном цвете
void N93_rainbow_zx (void)
{
  N93_Window(0,0,(N93_WIDTH/2)-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[0]);
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[1]);
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[2]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[3]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[4]);    
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[5]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[6]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[7]);  

  N93_Window((N93_WIDTH/2),0,N93_WIDTH-1,N93_HEIGHT-1);
  N93_write_command(N93_MEMORY_WRITE);
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[8]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[9]);    
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[10]);    
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[11]);    
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[12]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[13]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[14]);  
  N93_fill16(16,(N93_WIDTH/2),ZX_ATTR[15]);  
  
}


//-------------------------------------------------
// LCDSetLine.c 
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// Inputs: x = row address (0 .. 320) 
// y = column address (0 .. 320) 
// color = 16-bit color value

void N93_draw(unsigned int x0, unsigned int y0, unsigned int x1, 
                     unsigned int y1, unsigned int color) 
{ 
 int dy = y1 - y0; 
 int dx = x1 - x0; 
 int stepx, stepy; 

 if (dy < 0) 
   { dy = -dy; 
      stepy = -1; 
   } 
 else { stepy = 1; } 

 if (dx < 0) { dx = -dx; stepx = -1; } else { stepx = 1; } 
 dy <<= 1; // dy is now 2*dy 
 dx <<= 1; // dx is now 2*dx 
 
 N93_putpixel16(x0, y0, color); 

 if (dx > dy) 
  { 
   int fraction = dy - (dx >> 1); // same as 2*dy - dx 
   while (x0 != x1) 
   { 
     if (fraction >= 0) 
     { 
       y0 += stepy; 
       fraction -= dx; // same as fraction -= 2*dx 
     } 
     x0 += stepx; 
     fraction += dy; // same as fraction -= 2*dy 
     N93_putpixel16(x0, y0, color); 
    } 
   } 
 else 
  { 
   int fraction = dx - (dy >> 1); 
   while (y0 != y1) 
   { 
     if (fraction >= 0) 
     { 
       x0 += stepx; 
       fraction -= dy; 
     } 
     y0 += stepy; 
     fraction += dx; 
     N93_putpixel16(x0, y0, color); 
    } 
  } 
} 

//------------------------------------------------------------------
// Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
// Прямоугоольник может быть закрашен
// 
// Inputs: x = row address (0 .. 320) 
// y = column address (0 .. 320) 
// fill = 0-не закрашеный, 1-закрашеный
// color = 16-bit color value
 
void N93_setrect(unsigned int x0, unsigned int y0, unsigned int x1, 
                     unsigned int y1, unsigned char fill, unsigned int color) 
{ 
 int xmin, xmax, ymin, ymax; 
 int i; 
 
 // Если 1, то прямоугольник закрашиваем
 if (fill == 1) 
 { 
  // best way to create a filled rectangle is to define a drawing box 
  // and loop two pixels at a time 
  // calculate the min and max for x and y directions 
  xmin = (x0 <= x1) ? x0 : x1; 
  xmax = (x0 > x1) ? x0 : x1; 
  ymin = (y0 <= y1) ? y0 : y1; 
  ymax = (y0 > y1) ? y0 : y1; 
 
  // specify the controller drawing box according to those limits 
  // Row address set (command 0x2B) 
  N93_write_command(N93_COLUMN_ADDRESS_SET); // Set x
  N93_write_data(xmin>>8);
  N93_write_data(xmin);
  N93_write_data(xmax>>8);
  N93_write_data(xmax);

  N93_write_command(N93_PAGE_ADDRESS_SET);  // Set y
  N93_write_data(ymin>>8);
  N93_write_data(ymin);
  N93_write_data(ymax>>8);
  N93_write_data(ymax);

  N93_write_command(N93_MEMORY_WRITE);
 
  // loop on total number of pixels / 2 
  for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++) 
    { 
      // use the color value to output three data bytes covering two pixels 
      N93_write_data((unsigned char)(color>>8));
      N93_write_data((unsigned char)(color));
    } 
 
   } 
 else 
 { 
  // best way to draw un unfilled rectangle is to draw four lines 
  N93_draw(x0, y0, x1, y0, color); 
  N93_draw(x0, y1, x1, y1, color); 
  N93_draw(x0, y0, x0, y1, color); 
  N93_draw(x1, y0, x1, y1, color); 
 }
} 

//-----------------------------------------------------------------
// Draws a line in the specified color at center (x0,y0) with radius 
// 
// Inputs: x0 = row address (0 .. 131) 
// y0 = column address (0 .. 131) 
// radius = radius in pixels 
// color = 16-bit color value
// 
// Author: Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// Note: taken verbatim Wikipedia article on Bresenham's line algorithm 
// http://www.wikipedia.org 
 
void N93_circle(unsigned int x0, unsigned int y0, 
                    unsigned char radius, unsigned int color) 
{ 
 int f = 1 - radius; 
 int ddF_x = 0; 
 int ddF_y = -2 * radius; 
 int x = 0; 
 int y = radius; 
 
 N93_putpixel16(x0, y0 + radius, color); 
 N93_putpixel16(x0, y0 - radius, color); 
 N93_putpixel16(x0 + radius, y0, color); 
 N93_putpixel16(x0 - radius, y0, color); 
 
 while(x < y) 
 { 
   if(f >= 0) 
   { 
     y--; 
     ddF_y += 2; 
     f += ddF_y; 
   } 
   x++; 
   ddF_x += 2; 
   f += ddF_x + 1; 
   N93_putpixel16(x0 + x, y0 + y, color); 
   N93_putpixel16(x0 - x, y0 + y, color); 
   N93_putpixel16(x0 + x, y0 - y, color); 
   N93_putpixel16(x0 - x, y0 - y, color); 
   N93_putpixel16(x0 + y, y0 + x, color); 
   N93_putpixel16(x0 - y, y0 + x, color); 
   N93_putpixel16(x0 + y, y0 - x, color); 
   N93_putpixel16(x0 - y, y0 - x, color); 
 } 
} 


//----------------
// Печать символа
// Size: 0,1,2

void N93_putchar(unsigned char c, unsigned int x, unsigned int y, 
                     unsigned char size, unsigned int fColor, unsigned int bColor) 
{ 

  unsigned char i,j; 
  unsigned char nCols; 
  unsigned char nRows; 
  unsigned char nBytes; 
  unsigned char ByteRow; 
  unsigned char Mask; 
  unsigned int color; 
  unsigned char pChar[16]; 

  switch(size) 
  { 
     case 0: 
        nCols = FONT6x8[0][0]; 
        nRows = FONT6x8[0][1]; 
        nBytes= FONT6x8[0][2]; 
        memcpy(pChar, FONT6x8[c - 0x1F], nBytes); 
     break; 
       
     case 1: 
        nCols = FONT8x8[0][0]; 
        nRows = FONT8x8[0][1]; 
        nBytes= FONT8x8[0][2]; 
        memcpy(pChar, FONT8x8[c - 0x1F], nBytes); 
     break; 
       
     case 2: 
        nCols = FONT8x16[0][0]; 
        nRows = FONT8x16[0][1]; 
        nBytes= FONT8x16[0][2]; 
        memcpy(pChar, FONT8x16[c - 0x1F], nBytes); 
     break; 
  } 
  if ((bColor != TRANSPARENT))
    // Если цвет не прозрачный, то используем более быстрый алгоритм вывода
  {
    N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
    N93_write_data(y>>8);
    N93_write_data(y);
    N93_write_data((y + nRows -1)>>8);
    N93_write_data(y + nRows -1);

    N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
    N93_write_data(x>>8);
    N93_write_data(x);
    N93_write_data((x + nCols - 1)>>8);
    N93_write_data(x + nCols - 1);

    N93_write_command(N93_MEMORY_WRITE);

    for (i=0; i < nRows; i++) // 8 or 16
    { 
     ByteRow = pChar[i];
     Mask = 0x80; 
     for (j = 0; j < nCols; j++) // 6 or 8
      { 
        if ((ByteRow & Mask) != 0)   color = fColor; 
          else        color = bColor; 
        Mask = Mask >> 1; 
        N93_write_data((unsigned char)(color>>8));
        N93_write_data((unsigned char)(color));
      } 
    }
    N93_write_command(N93_NOP); // terminate the Write Memory command 
  }
  else // Если цвет прозрачный, то используем медленный алгоритм вывода "поточечно"
  {
  for (i=0; i < nRows; i++) // 8 or 16
    { 
     ByteRow = pChar[i];
     Mask = 0x80; 
     for (j = 0; j < nCols; j++) // 6 or 8
      { 
        if ((ByteRow & Mask) != 0)   color = fColor; 
          else        color = bColor; 
        Mask = Mask >> 1; 
        if (color != TRANSPARENT) N93_putpixel16(x+j,y+i,color); // Если цвет не прозрачный, то выводим
      } 
    }
  } 
    
} 


//-------------------------------------------------------------
//  N93_putstr("Temperature",55,86,2,BLUE_COLOR,TRANSPARENT);  

void N93_putstr(char *pString, unsigned int x, unsigned int y, 
                    unsigned char Size, unsigned int fColor, unsigned int bColor) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
       // draw the character 
       N93_putchar(*pString++, x, y, Size, fColor, bColor); 
       // advance the y position 
       if (Size == SMALL_FONT) // if SMALL
          x = x + 6; 
       else 
          x = x + 8; 
       if (y > N93_HEIGHT) break; 
    } 
} 

//------------------------------------------------------
// Вывод стандартной картинки .scr ZX Spectrum построчно
// input: Pointer to screen
// Use putpixel (slow algorithm)

void DrawScreen_zx(const unsigned char *pScreen)
{
  unsigned char x, y, line, byte_img, byte_attr, col, row;
  unsigned char ink, paper;
  unsigned int p;
  y=0;
  for (row=0; row<24; row++) // Цикл строк
  {
    p=ZX_ADDR[row]; // next addr pointer
    for(line=0; line<8; line++) // Вывод одной строки 8 точек
    {
      x=0;
      for(col=0; col<32; col++) // Цикл знакоместа
      {
        byte_img = pScreen [p++]; // Каждый байт картинки
        byte_attr = pScreen [6144 + (32*row) + col]; // Атрибут байта
        ink = (byte_attr&0x07); // Цвет чернил
        paper = ((byte_attr>>3)&0x07); // Цвет фона

        if (byte_attr&0x40)     // Определение яркости атрибута
          {
            ink = ink + 8;
            paper = paper + 8;
          }

        if (byte_img & 0x80) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x40) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x20) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x10) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x08) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x04) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x02) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
        if (byte_img & 0x01) N93_putpixel16(x,y,ZX_ATTR[ink]);
          else               N93_putpixel16(x,y,ZX_ATTR[paper]);
        x++;
      }
      p=p+256-32; // Next line addr (increment high byte)
      y++;
    }
  }
}

//--------------------------------------
// Draw 8x8 pixels of standart ZX screen

void Draw8x8 (unsigned char col, unsigned char row, const unsigned char *pScreen)
{
  unsigned int p;
  unsigned char ink, paper;
  unsigned char line, byte_img, byte_attr;  

  // Задаем квадрат заполнения 8х8 пикселей
  N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
  N93_write_data((row*8)>>8);
  N93_write_data(row*8);
  N93_write_data(((row*8) + 7)>>8);
  N93_write_data((row*8) + 7);

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
  N93_write_data((col*8)>>8);
  N93_write_data(col*8);
  N93_write_data(((col*8) + 7)>>8);
  N93_write_data((col*8) + 7);

  N93_write_command(N93_MEMORY_WRITE);

  p=ZX_ADDR[row]+col; // адрес знакоместа

  byte_attr = pScreen [6144 + (32*row) + col]; // Атрибут знакоместа берем один раз
  ink = (byte_attr&0x07); // Цвет чернил
  paper = ((byte_attr>>3)&0x07); // Цвет фона
  if (byte_attr&0x40)     // Определение яркости атрибута
   {
     ink = ink + 8;
     paper = paper + 8;
   }

  for (line=0; line<8; line++)
  {
        byte_img = pScreen [p]; // байт картинки

        if (byte_img & 0x80) 
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x40)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x20)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x10)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x08)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x04)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x02)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x01)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
      p=p+256; // Next line addr (increment high byte)    
  }
}

//--------------------------------------------------------
// Вывод стандартной картинки .scr ZX Spectrum посимвольно
// input: Pointer to screen
// Use fill 8x8 (fast algorithm)

void DrawScreen2_zx(const unsigned char *pScreen)
{
  unsigned char col, row;

  for (row=0; row<24; row++) // Цикл строк
  {
    for(col=0; col<32; col++) // Цикл знакоместа
    {
      Draw8x8(col,row,pScreen);
    }
  }
}


//---------------------------------------------------------
// Вывод черно-белого спрайта заданного размера
// в заданных координатах без маски

void DrawSpriteBW (unsigned int x, unsigned int y, unsigned int numsprite,
                   unsigned char width, unsigned char height, const unsigned char *pSprites)
{
  unsigned int p; // Указатель на спрайт
  p=width*height*numsprite;

  unsigned int ink, paper;
  ink = WHITE_COLOR;
  paper = BLACK_COLOR;
  
  unsigned int bytes; // Количество байт спрайта
  unsigned char byte_img;
  
  N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data((y + height - 1)>>8);
  N93_write_data(y + height - 1);

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data((x + (width*8) - 1)>>8);
  N93_write_data(x + (width*8) - 1);

  N93_write_command(N93_MEMORY_WRITE);

  for (bytes=0; bytes<(width*height); bytes++) // Цикл выборки байтов из спрайта
    {
        byte_img = pSprites [p++]; // байт картинки

        if (byte_img & 0x80) 
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x40)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x20)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x10)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x08)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x04)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x02)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x01)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          } 
    }

}


//---------------------------------------------------------
// Вывод черно-белого спрайта заданного размера
// с нестандартной адресацией
// в заданных координатах без маски

void DrawSpriteNonStandard (unsigned int x, unsigned int y, unsigned int numsprite,
                   unsigned char width, unsigned char height, const unsigned char *pSprites)
{
  unsigned int p; // Указатель на спрайт
  p=width*height*numsprite;

  unsigned int ink, paper;
  ink = WHITE_COLOR;
  paper = BLACK_COLOR;
  
  unsigned char bytes, lines;
  unsigned char byte_img;
  
  N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data((y + height - 1)>>8);
  N93_write_data(y + height - 1);

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data((x + (width*8) - 1)>>8);
  N93_write_data(x + (width*8) - 1);

  N93_write_command(N93_MEMORY_WRITE);

  for (lines=0; lines < height; lines++)
  {
    for (bytes=0; bytes < width; bytes++) // Цикл выборки байтов из спрайта
    {
        byte_img = pSprites [p++]; // байт картинки

        if (byte_img & 0x80) 
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x40)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x20)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x10)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x08)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x04)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x02)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
        if (byte_img & 0x01)
          {
            N93_write_data(ink>>8);
            N93_write_data(ink);
          }
          else               
          {
            N93_write_data(paper>>8);
            N93_write_data(paper);
          }
    }
    p=p+32-width; // нестандартная адресация
  }

}


//-----------------------------------
// Стирание спрайта по координатам,
// заданного размера и цвета
// width в пикселях
void ClearSprite (unsigned int x, unsigned int y,
                   unsigned char width, unsigned char height, unsigned int color)
{
  unsigned int pixels;
  
  N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data((y + height - 1)>>8);
  N93_write_data(y + height - 1);

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data((x + width - 1)>>8);
  N93_write_data(x + width - 1);

  N93_write_command(N93_MEMORY_WRITE);

  for (pixels=0; pixels<(width*height); pixels++) // Цикл стирания пикселей
    {
      N93_write_data(color>>8);
      N93_write_data(color);
    }

}

//---------------------------------------------------------
// Вывод спрайта игры Nodes of Yesod
// в заданных координатах без маски.
// Массив спрайтов содержит три байта перед каждым спрайтом:
// 1 - x_size in pixels
// 2 - y_sixe in pixels
// 3 - color attribute

void DrawSpriteNodes (unsigned int x, unsigned int y, unsigned int numsprite,
                      const unsigned char *pSprites)
{
  unsigned char width_bytes, width, height; // size of sprite
  unsigned char attribute; // color of sprite
  unsigned int i;
  unsigned int p=0; // Указатель на спрайт
  if (numsprite>0) // Поиск текущего спрайта вычислением
  {
    for (i=0; i<numsprite; i++)
    {
      width = pSprites[p++];
      height = pSprites[p++];

      width_bytes = width>>3;
      if (width%8) width_bytes++;

      p=p + (width_bytes*height) + 1; // И +1 (байт атрибута учитываем)
    }
  }


  width = pSprites[p++];        // 1 byte - width
  height = pSprites[p++];       // 2 byte - height
  attribute = pSprites[p++];    // 3 byte - attribute

  unsigned int ink, paper;
  ink = (attribute&0x07); // Цвет чернил
  paper = ((attribute>>3)&0x07); // Цвет фона
  if (attribute&0x40)     // Определение яркости атрибута
   {
     ink = ink + 8;
     paper = paper + 8;
   }

//*********************
//  paper++; //!!!DEBUG
//*********************

  //деление целого на 8 с округлением в большую сторону
  width_bytes = width>>3;
  if (width%8) width_bytes++;

//***********************************    
//  usprintf(LN6,"WIDTH=%02X", width_bytes);    //!!!DEBUG
//***********************************  

  unsigned int bytes; // Количество байт спрайта
  unsigned char byte_img;
  
  N93_write_command(N93_PAGE_ADDRESS_SET);  // Page (Row) address set (y)
  N93_write_data(y>>8);
  N93_write_data(y);
  N93_write_data((y + height - 1)>>8);
  N93_write_data(y + height - 1);

  N93_write_command(N93_COLUMN_ADDRESS_SET); // Column address set (x)
  N93_write_data(x>>8);
  N93_write_data(x);
  N93_write_data((x + (width_bytes*8) - 1)>>8);
  N93_write_data(x + (width_bytes*8) - 1);

  N93_write_command(N93_MEMORY_WRITE);

  for (bytes=0; bytes<(width_bytes*height); bytes++) // Цикл выборки байтов из спрайта
    {
        byte_img = pSprites [p++]; // байт картинки

        if (byte_img & 0x80) 
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x40)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x20)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x10)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x08)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x04)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x02)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x01)
          {
            N93_write_data((unsigned char)(ZX_ATTR[ink]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            N93_write_data((unsigned char)(ZX_ATTR[paper]>>8));
            N93_write_data((unsigned char)(ZX_ATTR[paper]));
          }
    }

//****************** DEBUG ***************************  
//  usprintf(LN2,"%ux%u", width, height);
//  N93_putstr(LN2, x, y+height+1, SMALL_FONT,CYAN_COLOR,BLACK_COLOR);
//****************************************************

}

//-------------------------------------------------------------
//  OLLI_putstr("Temperature",55,86);
// Печать стринга шрифтом OLLI по координатам x, y

void OLLI_putstr(char *pString, unsigned int x, unsigned int y) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
      // draw the character 
      DrawSpriteBW (x,y,(*pString++)-0x20,1,8,FONT_OLLI);
      // advance the y position 
      x = x + 8;
      if (x > N93_WIDTH) x=N93_WIDTH;
      if (y > N93_HEIGHT) break; 
    } 
} 

/*
//------------------------------------------
// Чтение ID из ЖКИ
void print_command (unsigned char command)
{
  unsigned char id1,id2,id3,id4,id5;
  N93_write_command(command);
  id1=N93_read_data();
  id2=N93_read_data();
  id3=N93_read_data();
  id4=N93_read_data();
  id5=N93_read_data();
  usprintf(LN1,"ID1=%02X",id1);  // FF 26 
  usprintf(LN2,"ID2=%02X",id2);  // 83 83
  usprintf(LN3,"ID3=%02X",id3);  // 8A 8A
  usprintf(LN4,"ID4=%02X",id4);  // 27 27
  usprintf(LN5,"ID5=%02X",id5);  // 00 00
  //  pcf8814_putchar16(0x30);
  pcf8814_update_console();
}
*/


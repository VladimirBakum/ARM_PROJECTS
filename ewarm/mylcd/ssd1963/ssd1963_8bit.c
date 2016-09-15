//============================================
// SSD1963 code for ITDB02-4.3 LCM
// 8-bit version
// By (R)soft 6-03-2015 ...
// Version 1.0
//============================================


//#include <math.h> // for floor, trunc, round, ceil, etc..
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"

#include "fonts.h" // 3 Fonts
#include "ssd1963_8bit.h"


#include "utils/ustdlib.h" // for usprintf

//=============================================================
// Запись команды в ЖКИ

void ssd1963_wr_cmd (unsigned char byte)
{
  DCX_0; // Режим команды
  WRX_0; // WRX=0 - Write mode
  
  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;

  WRX_1; // байт передается в контроллер
  DCX_1; // возвращаем основной режим - режим записи данных
}

//=============================================================
// Запись данных в ЖКИ

void ssd1963_wr_dat (unsigned char byte)
{
  WRX_0; // WRX=0 - Write mode

  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;

  WRX_1; // байт передается в контроллер
}

//=============================================================
// Чтение байта из ЖКИ
// RS при чтении всегда 1
// Эта функция писалась исключительно для отладки кода и
// отображения отклика ssd1963 - чтение регистров.

unsigned char ssd1963_rd_dat(void)
{
  unsigned char data=0xFF;
  
  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_IN);  // Шина данных на ввод
  
  RDX_0;  // RDX=0 - Read mode  Строб чтения - данные выставились
  
//  data = GPIOPinRead(PORT_DATA, 0xFF);  // Чтение с шины данных
    data = HWREG(PORT_DATA +  (0xFF << 2));
  
  //    data = HWREG(PORT_DATA + (GPIO_O_DATA + (0xFF << 2)));
    
  RDX_1;  // Возврат RDX в исходное состояние

  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_OUT);  // Шина данных опять на вывод.

  return (data);
}


//-------------------------------------------------------
// Установка окна доступа в память дисплея
// При работе с графикой и выводом, правильно всегда
// обращаться через эту процедуру т.к. она учитывает
// невидимые области как самого LCD так и тачпанели.
// После установки тачпанели, стали не видны 2 линии вверху и 3 линии внизу.
// (Верх имеется в виду та сторона, которая ближе к чипу ILI9481).
// Таким образом, косвенно мы потеряли 5 линий дисплея.

void ssd1963_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  
#if (defined PORTRAIT) || (defined PORTRAIT_FLIP)
  ssd1963_wr_cmd(SSD1963_COLUMN_ADDRESS_SET); // Set x
  ssd1963_wr_dat(x1>>8); // Учитывается начальная невидимая область по x
  ssd1963_wr_dat(x1);
  ssd1963_wr_dat(x2>>8);
  ssd1963_wr_dat(x2);

  ssd1963_wr_cmd(SSD1963_PAGE_ADDRESS_SET);  // Set y
  ssd1963_wr_dat(y1>>8);
  ssd1963_wr_dat(y1);
  ssd1963_wr_dat(y2>>8);
  ssd1963_wr_dat(y2);
#else
  ssd1963_wr_cmd(SSD1963_COLUMN_ADDRESS_SET); // Set x
  ssd1963_wr_dat(x1>>8);
  ssd1963_wr_dat(x1);
  ssd1963_wr_dat(x2>>8);
  ssd1963_wr_dat(x2);

  ssd1963_wr_cmd(SSD1963_PAGE_ADDRESS_SET);  // Set y
  ssd1963_wr_dat(y1>>8);
  ssd1963_wr_dat(y1);
  ssd1963_wr_dat(y2>>8);
  ssd1963_wr_dat(y2);
#endif
  
}

//-----------------------------------------------------
// Очистка дисплея

void ssd1963_clear(unsigned long color)
{
  unsigned int x,y;
  ssd1963_Window(0, 0, SSD1963_WIDTH-1, SSD1963_HEIGHT-1);
  ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);

  for (x = SSD1963_PIXELX; x; x--) 
   {
       for (y = SSD1963_PIXELY; y; y--) 
       {
         ssd1963_wr_dat((unsigned char)(color>>8));
         ssd1963_wr_dat((unsigned char)color);
       }
   }

  ssd1963_Window(0, 0, SSD1963_WIDTH-1, SSD1963_HEIGHT-1);

}

//-----------------------------------------
// Установка точки 16 bit/pixel

void ssd1963_putpixel(unsigned int x, unsigned int y, unsigned int color) 
{ 
  ssd1963_Window(x, y, x, y);
  ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);

  ssd1963_wr_dat((unsigned char)(color>>8));
  ssd1963_wr_dat((unsigned char)color);

} 


//=======================================
// Заполнение n строк одним цветом 16 bit/pixel
void ssd1963_fill (unsigned int lines, unsigned int color)
{    
 unsigned int a,b;   

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(SSD1963_WIDTH);b++)
         {
           ssd1963_wr_dat((unsigned char)(color>>8));
           ssd1963_wr_dat((unsigned char)color);
         }
   }

}

//-----------------------------
// Вывод радуги  в 16-битном цвете
// Расчет под PORTRAIT mode
void ssd1963_rainbow (void)
{
  ssd1963_Window(0,0,SSD1963_WIDTH-1,SSD1963_HEIGHT-1);
  ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);

  ssd1963_fill(32,OLIVE_COLOR);
  ssd1963_fill(32,BLUE_COLOR);
  ssd1963_fill(32,PURPLE_COLOR);  
  ssd1963_fill(32,CYAN_COLOR);  
  ssd1963_fill(32,YELLOW_COLOR);
  ssd1963_fill(32,RED_COLOR);
  ssd1963_fill(32,GREEN_COLOR);
  ssd1963_fill(32,BLACK_COLOR);  
  ssd1963_fill(32,ORANGE_COLOR);
  ssd1963_fill(32,PINK_COLOR);
  ssd1963_fill(32,WHITE_COLOR);
  ssd1963_fill(64,NAVY_COLOR);    
  ssd1963_fill(64,YELLOW_COLOR);  
}

//-------------------------------------------------
// === Draw Line ===
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// Inputs: x = row address;
// y = column address;
// color = 16-bit color value

void ssd1963_draw(unsigned int x0, unsigned int y0, unsigned int x1, 
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
 
 ssd1963_putpixel(x0, y0, color); 

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
     ssd1963_putpixel(x0, y0, color); 
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
     ssd1963_putpixel(x0, y0, color); 
    } 
  } 
} 

//------------------------------------------------------------------
// === Set Rectangle ===
// Draws a rectangle in the specified color from (x1,y1) to (x2,y2) 
// Rectangle can be filled with a color if desired 
// Inputs: x = row address
// y = column address
// fill = 0=no fill, 1-fill entire rectangle 
// color = 16-bit color value
 
void ssd1963_setrect(unsigned int x0, unsigned int y0, unsigned int x1, 
                     unsigned int y1, unsigned char fill, unsigned int color) 
{ 
  unsigned int xmin, xmax, ymin, ymax; 
  unsigned int i; 
 
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

  ssd1963_Window(xmin, ymin, xmax, ymax);
  ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);
 
  // loop on total number of pixels
  for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++) 
    { 
      // use the color value to output three data bytes covering two pixels 
      ssd1963_wr_dat((unsigned char)(color>>8));
      ssd1963_wr_dat((unsigned char)color);
    }
  
  ssd1963_wr_cmd(SSD1963_NOP);
  
  } 
  else 
  { 
    // best way to draw un unfilled rectangle is to draw four lines 
    ssd1963_draw(x0, y0, x1, y0, color); 
    ssd1963_draw(x0, y1, x1, y1, color); 
    ssd1963_draw(x0, y0, x0, y1, color); 
    ssd1963_draw(x1, y0, x1, y1, color); 
  }
} 

//------------------------------------------------------------------
// === Circle ===
// Draws a line in the specified color at center (x0,y0) with radius 
// Inputs: x0 = row address
// y0 = column address
// radius = radius in pixels 
// color = 16-bit color value
// Author: Jack Bresenham IBM, Winthrop University (Father of this algorithm, 1962) 
// Note: taken verbatim Wikipedia article on Bresenham's line algorithm 
// http://www.wikipedia.org 
 
void ssd1963_circle(unsigned int x0, unsigned int y0, 
                    unsigned char radius, unsigned int color) 
{ 
  int f = 1 - radius; 
  int ddF_x = 0; 
  int ddF_y = -2 * radius; 
  int x = 0; 
  int y = radius; 
 
  ssd1963_putpixel(x0, y0 + radius, color); 
  ssd1963_putpixel(x0, y0 - radius, color); 
  ssd1963_putpixel(x0 + radius, y0, color); 
  ssd1963_putpixel(x0 - radius, y0, color); 
 
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
    ssd1963_putpixel(x0 + x, y0 + y, color); 
    ssd1963_putpixel(x0 - x, y0 + y, color); 
    ssd1963_putpixel(x0 + x, y0 - y, color); 
    ssd1963_putpixel(x0 - x, y0 - y, color); 
    ssd1963_putpixel(x0 + y, y0 + x, color); 
    ssd1963_putpixel(x0 - y, y0 + x, color); 
    ssd1963_putpixel(x0 + y, y0 - x, color); 
    ssd1963_putpixel(x0 - y, y0 - x, color); 
  } 
} 


//---------------------------------------------------
// Установка цвета чернил и фона через процедуру
void ssd1963_setcolor (unsigned int foreground, unsigned int background)
{
  fColor=foreground;
  bColor=background;
}


//----------------------------------------
// утстановка координат через процедуру
void  ssd1963_setpos(unsigned int x_pos, unsigned int y_pos)
{
  x=x_pos;
  y=y_pos;
}


//------------------------------------------------
// Печать символа
// fontsize: 0,1,2

void ssd1963_putchar(unsigned char c) 
{ 

  unsigned char i,j; 
  unsigned char nCols; 
  unsigned char nRows; 
  unsigned char nBytes; 
  unsigned char ByteRow; 
  unsigned char Mask; 
  unsigned int color; 
  unsigned char pChar[16]; 

  switch(fontsize) 
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
  if ((bColor != TRANSPARENT)) //  if ((bColor != TRANSPARENT)||(fColor != TRANSPARENT))
    // Если цвет не прозрачный, то используем более быстрый алгоритм вывода
  {
    ssd1963_Window(x,y,(x + nCols - 1),(y + nRows -1));
    ssd1963_wr_cmd(SSD1963_MEMORY_WRITE);

    for (i=0; i < nRows; i++) // 8 or 16
    { 
     ByteRow = pChar[i];
     Mask = 0x80; 
     for (j = 0; j < nCols; j++) // 6 or 8
      { 
        if ((ByteRow & Mask) != 0)   color = fColor; 
          else        color = bColor; 
        Mask = Mask >> 1; 
        ssd1963_wr_dat((unsigned char)(color>>8));
        ssd1963_wr_dat((unsigned char)(color));
      } 
     }
    ssd1963_wr_cmd(SSD1963_NOP); // terminate the Write Memory command 
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
        // Если цвет не прозрачный, то выводим
        if (color != TRANSPARENT) ssd1963_putpixel(x+j, y+i, color);
      } 
    }
  } 
    
} 


//-------------------------------------------------------------
//  ili9481_putstr("Temperature");  

void ssd1963_putstr(char *pString) 
{ 
  while (*pString != '\0') // loop until null-terminator is seen 
    {
      if (*pString=='\n')  // Enter symbol not print
        { 
          *pString++; 
          x = 0;
          if (fontsize==0) y = y + 10;
           else            y = y + 16; 
        } 
      else
        {
          ssd1963_putchar(*pString++); // draw the character 

          if (fontsize==0)  x = x + 6; // if small font
          else              x = x + 8;

          if (x > (SSD1963_WIDTH-1)) 
            { 
              x = 0;
              if (fontsize==0) y = y + 10;
               else            y = y + 16; 
            }
       
          if (y > (SSD1963_HEIGHT-1)) y=0; 
        }
    } 
} 


//------------------------------------
// Initialize SSD1963
// Specified for ITDB02-4.3 LCD

void ssd1963_init(void)
{
  // Настройка портов LCD
  // ---------- Порт управления ----------------
  SysCtlPeripheralEnable(SYS_CTRL);
  SysCtlGPIOAHBEnable(SYS_CTRL); // Включить режим Advanced
  GPIOPinTypeGPIOOutput( PORT_CTRL, (PIN_DCX | PIN_WRX | PIN_RESETX | PIN_RDX | PIN_CSX));
  GPIOPadConfigSet( PORT_CTRL, (PIN_DCX | PIN_WRX | PIN_RESETX | PIN_RDX | PIN_CSX), \
                   GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

  // ------------- Порт данных -----------------
  SysCtlPeripheralEnable(SYS_DATA);
  SysCtlGPIOAHBEnable(SYS_DATA); // Включить режим Advanced
  GPIOPinTypeGPIOOutput(PORT_DATA, 0xFF );
  GPIOPadConfigSet(PORT_DATA, 0xFF, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  RDX_1;  // Исходное состояние Read=1
  WRX_1;  // Исходное состояние Write=1
  DCX_1; // Исходно DCX всегда 1 (запись данных)
  CSX_0; // Chip Select всегда выбран

  RESETX_0;  // Hardware Reset LCD
  delay_ms(20);
  RESETX_1;  // Return Reset OFF
  delay_ms(150);
  
  ssd1963_wr_cmd(SSD1963_SLEEP_OUT); // Exit Sleep Mode 0x11
  delay_ms(100);  // Wait Stability
		
  ssd1963_wr_cmd(SSD1963_NORMAL_DISPLAY_MODE_ON); // Entering Nomal Displaymode 0x13

  /*
  ili9481_wr_cmd(ILI9481_VCOM_CONTROL); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x3F); // Значения подбирать опытным путем до естественной цветопередачи
  ili9481_wr_dat(0x0B);
  
  

  ili9481_wr_cmd(ILI9481_FRAME_RATE_AND_INVERSION_CTRL); // Frame Rate and Inversion Control
  ili9481_wr_dat(0x00); // 125 Гц (На других частотах мельтишит)
*/  

  ssd1963_wr_cmd(SSD1963_SET_ADDRESS_MODE); // Set_address_mode 0x36
// B7         B6         B5              B4                B3        B2                B1           B0
// направл Y, направл X, выбор X или Y, Vertical направл, RGB/BGR=1, DisplDataLatch=0, Horiz Flip,  Vert Flip

  ssd1963_wr_dat(0x08|LCD_ORIENTATION); // BGR-order (0x88)

  
  ssd1963_wr_cmd(SSD1963_SET_PIXEL_FORMAT); // Set_pixel_format 0x3A
  ssd1963_wr_dat(0x55); // RGB565(16dpp) 16-битный цвет


  ssd1963_wr_cmd(SSD1963_DISPLAY_ON); // Display ON

}

//--------------------------------------------------
// Аналог функции usprintf сразу с выводом на LCD
// По типу print formatter (замена UARTprinf)
// Usage: ili9481_printf("1:%u 2:%u 3:%x",x,y,z);

int ssd1963_printf(const char *pcString, ...)
{
  va_list vaArgP;
  int iRet;
  char *pcBuf=BUFER;
  //
  // Start the varargs processing.
  //
  va_start(vaArgP, pcString);

  //
  // Call vsnprintf to perform the conversion.  Use a large number for the
  // buffer size.
  //
  iRet = uvsnprintf(pcBuf, 0xffff, pcString, vaArgP);
  ssd1963_putstr(pcBuf); // Out to LCD
  
  //
  // End the varargs processing.
  //
  va_end(vaArgP);

  //
  // Return the conversion count.
  //
  return(iRet);
}


//-----------------------------------
// Узор1
void draw_pattern1 (void)
{
  unsigned int i;
  for (i=0; i<(SSD1963_HEIGHT-1); i=i+10)
    {
      ssd1963_draw(0,(SSD1963_HEIGHT-1),(SSD1963_WIDTH-1),i, RED_COLOR);
    }
  for (i=0; i<(SSD1963_HEIGHT-1); i=i+10)
    {
      ssd1963_draw((SSD1963_WIDTH-1),0,0,i, YELLOW_COLOR);
    }  
}



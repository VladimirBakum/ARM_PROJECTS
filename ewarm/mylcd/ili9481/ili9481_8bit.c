//============================================
// ILI9481 code for TFT8K2346 & TFT8K5029 LCMs
// 8-bit version (IM0=1 & IM1=1)
// By (R)soft 16-04-2014 ... 01-05-2014
// Version 1.1
//============================================


//#include <math.h> // for floor, trunc, round, ceil, etc..
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"

#include "fonts.h" // 3 Fonts
#include "ili9481_8bit.h"


#include "utils/ustdlib.h" // for usprintf

//=============================================================
// Запись команды в ЖКИ

void ili9481_wr_cmd (unsigned char byte)
{
  DCX_0; // Режим команды
  DCX_0;
  
  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;

  WRX_0; // WRX=0 - Write mode
  WRX_0;
  WRX_1; // байт передается в контроллер

  DCX_1; // возвращаем основной режим - режим записи данных
  
}

//=============================================================
// Запись данных в ЖКИ

void ili9481_wr_dat (unsigned char byte)
{
  // выставляем байт на шину
  HWREG(PORT_DATA | (0xFF << 2)) = byte;

  WRX_0; // WRX=0 - Write mode
  WRX_0;
  WRX_1; // байт передается в контроллер
  WRX_1;
}

//=============================================================
// Чтение байта из ЖКИ
// RS при чтении всегда 1
// Эта функция писалась исключительно для отладки кода и
// отображения отклика ILI9481 - чтение регистров.

unsigned char ili9481_rd_dat(void)
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

void ili9481_Window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

//  if(x2>=ILI9481_WIDTH) { x2 = ILI9481_WIDTH-1; }
//  if(y2>=ILI9481_HEIGHT){ y2 = ILI9481_HEIGHT-1; }
  
#if (defined PORTRAIT) || (defined PORTRAIT_FLIP)
  ili9481_wr_cmd(ILI9481_COLUMN_ADDRESS_SET); // Set x
  ili9481_wr_dat((x1+ILI9481_UNVISIBLE)>>8); // Учитывается начальная невидимая область по x
  ili9481_wr_dat(x1+ILI9481_UNVISIBLE);
  ili9481_wr_dat((x2+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(x2+ILI9481_UNVISIBLE);

  ili9481_wr_cmd(ILI9481_PAGE_ADDRESS_SET);  // Set y
  ili9481_wr_dat(y1>>8);
  ili9481_wr_dat(y1);
  ili9481_wr_dat(y2>>8);
  ili9481_wr_dat(y2);
#else
  ili9481_wr_cmd(ILI9481_COLUMN_ADDRESS_SET); // Set x
  ili9481_wr_dat(x1>>8);
  ili9481_wr_dat(x1);
  ili9481_wr_dat(x2>>8);
  ili9481_wr_dat(x2);

  ili9481_wr_cmd(ILI9481_PAGE_ADDRESS_SET);  // Set y
  ili9481_wr_dat((y1+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(y1+ILI9481_UNVISIBLE);
  ili9481_wr_dat((y2+ILI9481_UNVISIBLE)>>8);
  ili9481_wr_dat(y2+ILI9481_UNVISIBLE);
#endif
  
}

//-----------------------------------------------------
// Очистка дисплея

void ili9481_clear(unsigned long color)
{
  unsigned int x,y;
  ili9481_Window(0, 0, ILI9481_WIDTH-1, ILI9481_HEIGHT-1);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (x = ILI9481_PIXELX; x; x--) 
   {
       for (y = ILI9481_PIXELY; y; y--) 
       {
         ili9481_wr_dat((unsigned char)(color>>8));
         ili9481_wr_dat((unsigned char)color);
       }
   }

  ili9481_Window(0, 0, ILI9481_WIDTH-1, ILI9481_HEIGHT-1);

}

//-----------------------------------------
// Установка точки 16 bit/pixel

void ili9481_putpixel(unsigned int x, unsigned int y, unsigned int color) 
{ 
  ili9481_Window(x, y, x, y);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  ili9481_wr_dat((unsigned char)(color>>8));
  ili9481_wr_dat((unsigned char)color);

} 


//=======================================
// Заполнение n строк одним цветом 16 bit/pixel
void ili9481_fill (unsigned int lines, unsigned int color)
{    
 unsigned int a,b;   

 for(a=0;a<(lines);a++) // Количество строк
   {
      for(b=0;b<(ILI9481_WIDTH);b++)
         {
           ili9481_wr_dat((unsigned char)(color>>8));
           ili9481_wr_dat((unsigned char)color);
         }
   }

}

//-----------------------------
// Вывод радуги  в 16-битном цвете
// Расчет под PORTRAIT mode
void ili9481_rainbow (void)
{
  ili9481_Window(0,0,ILI9481_WIDTH-1,ILI9481_HEIGHT-1);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  ili9481_fill(32,OLIVE_COLOR);
  ili9481_fill(32,BLUE_COLOR);
  ili9481_fill(32,PURPLE_COLOR);  
  ili9481_fill(32,CYAN_COLOR);  
  ili9481_fill(32,YELLOW_COLOR);
  ili9481_fill(32,RED_COLOR);
  ili9481_fill(32,GREEN_COLOR);
  ili9481_fill(32,BLACK_COLOR);  
  ili9481_fill(32,ORANGE_COLOR);
  ili9481_fill(32,PINK_COLOR);
  ili9481_fill(32,WHITE_COLOR);
  ili9481_fill(64,NAVY_COLOR);    
  ili9481_fill(64,YELLOW_COLOR);  
}

//-------------------------------------------------
// === Draw Line ===
// Draws a line in the specified color from (x0,y0) to (x1,y1) 
// Inputs: x = row address;
// y = column address;
// color = 16-bit color value

void ili9481_draw(unsigned int x0, unsigned int y0, unsigned int x1, 
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
 
 ili9481_putpixel(x0, y0, color); 

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
     ili9481_putpixel(x0, y0, color); 
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
     ili9481_putpixel(x0, y0, color); 
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
 
void ili9481_setrect(unsigned int x0, unsigned int y0, unsigned int x1, 
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

  ili9481_Window(xmin, ymin, xmax, ymax);
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);
 
  // loop on total number of pixels
  for (i = 0; i < ((xmax - xmin + 1) * (ymax - ymin + 1)); i++) 
    { 
      // use the color value to output three data bytes covering two pixels 
      ili9481_wr_dat((unsigned char)(color>>8));
      ili9481_wr_dat((unsigned char)color);
    }
  
  ili9481_wr_cmd(ILI9481_NOP);
  
  } 
  else 
  { 
    // best way to draw un unfilled rectangle is to draw four lines 
    ili9481_draw(x0, y0, x1, y0, color); 
    ili9481_draw(x0, y1, x1, y1, color); 
    ili9481_draw(x0, y0, x0, y1, color); 
    ili9481_draw(x1, y0, x1, y1, color); 
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
 
void ili9481_circle(unsigned int x0, unsigned int y0, 
                    unsigned char radius, unsigned int color) 
{ 
  int f = 1 - radius; 
  int ddF_x = 0; 
  int ddF_y = -2 * radius; 
  int x = 0; 
  int y = radius; 
 
  ili9481_putpixel(x0, y0 + radius, color); 
  ili9481_putpixel(x0, y0 - radius, color); 
  ili9481_putpixel(x0 + radius, y0, color); 
  ili9481_putpixel(x0 - radius, y0, color); 
 
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
    ili9481_putpixel(x0 + x, y0 + y, color); 
    ili9481_putpixel(x0 - x, y0 + y, color); 
    ili9481_putpixel(x0 + x, y0 - y, color); 
    ili9481_putpixel(x0 - x, y0 - y, color); 
    ili9481_putpixel(x0 + y, y0 + x, color); 
    ili9481_putpixel(x0 - y, y0 + x, color); 
    ili9481_putpixel(x0 + y, y0 - x, color); 
    ili9481_putpixel(x0 - y, y0 - x, color); 
  } 
} 


//---------------------------------------------------
// Установка цвета чернил и фона через процедуру
void ili9481_setcolor (unsigned int foreground, unsigned int background)
{
  fColor=foreground;
  bColor=background;
}


//----------------------------------------
// утстановка координат через процедуру
void  ili9481_setpos(unsigned int x_pos, unsigned int y_pos)
{
  x=x_pos;
  y=y_pos;
}


//------------------------------------------------
// Печать символа
// fontsize: 0,1,2

void ili9481_putchar(unsigned char c) 
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
    ili9481_Window(x,y,(x + nCols - 1),(y + nRows -1));
    ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

    for (i=0; i < nRows; i++) // 8 or 16
    { 
     ByteRow = pChar[i];
     Mask = 0x80; 
     for (j = 0; j < nCols; j++) // 6 or 8
      { 
        if ((ByteRow & Mask) != 0)   color = fColor; 
          else        color = bColor; 
        Mask = Mask >> 1; 
        ili9481_wr_dat((unsigned char)(color>>8));
        ili9481_wr_dat((unsigned char)(color));
      } 
     }
    ili9481_wr_cmd(ILI9481_NOP); // terminate the Write Memory command 
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
        if (color != TRANSPARENT) ili9481_putpixel(x+j, y+i, color);
      } 
    }
  } 
    
} 


//-------------------------------------------------------------
//  ili9481_putstr("Temperature");  

void ili9481_putstr(char *pString) 
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
          ili9481_putchar(*pString++); // draw the character 

          if (fontsize==0)  x = x + 6; // if small font
          else              x = x + 8;

          if (x > (ILI9481_WIDTH-1)) 
            { 
              x = 0;
              if (fontsize==0) y = y + 10;
               else            y = y + 16; 
            }
       
          if (y > (ILI9481_HEIGHT-1)) y=0; 
        }
    } 
} 


//------------------------------------
// Initialize ILI9481
// Specified for TFT8K2346 LCD

void ili9481_init(void)
{
  // Настройка портов LCD
  // ---------- Порт управления ----------------
  SysCtlPeripheralEnable(SYS_CTRL);
  SysCtlGPIOAHBEnable(SYS_CTRL); // Включить режим Advanced
  GPIOPinTypeGPIOOutput( PORT_CTRL, (PIN_DCX | PIN_WRX | PIN_RESX | PIN_RDX));
  GPIOPadConfigSet( PORT_CTRL, (PIN_DCX | PIN_WRX | PIN_RESX | PIN_RDX), \
                   GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

  // ------------- Порт данных -----------------
  SysCtlPeripheralEnable(SYS_DATA);
  SysCtlGPIOAHBEnable(SYS_DATA); // Включить режим Advanced
  GPIOPinTypeGPIOOutput(PORT_DATA, 0xFF );
  GPIOPadConfigSet(PORT_DATA, 0xFF, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);
  
  RDX_1;  // Исходное состояние Read=1
  WRX_1;  // Исходное состояние Write=1
  DCX_1; // Исходно DCX всегда 1 (запись данных)

//  RESX_0;  // Hardware Reset LCD
  delay_ms(20);
  RESX_1;  // Return Reset OFF
  delay_ms(150);
  
  ili9481_wr_cmd(ILI9481_SOFTWARE_RESET);
  delay_ms(100);

  ili9481_wr_cmd(ILI9481_SLEEP_OUT); // Exit Sleep Mode 0x11
  delay_ms(100);  // Wait Stability
		
  ili9481_wr_cmd(ILI9481_NORMAL_DISPLAY_MODE_ON); // Entering Nomal Displaymode 0x13

  ili9481_wr_cmd(ILI9481_VCOM_CONTROL); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x3F); // Значения подбирать опытным путем до естественной цветопередачи
  ili9481_wr_dat(0x0B);
  
  
/*  
  ili9481_wr_cmd(ILI9481_POWER_SETTING); // LCD Power Settings
  ili9481_wr_dat(0x07); // 1.0xVci
  ili9481_wr_dat(0x40); // 40 41
  ili9481_wr_dat(0x1C); // 1c 1e

  ili9481_wr_cmd(ILI9481_POWER_SET_FOR_NOMAL_MODE); // Power_Setting for Normal Mode
  ili9481_wr_dat(0x01);
  ili9481_wr_dat(0x11); // 02 11

  ili9481_wr_cmd(ILI9481_PANEL_DRIVING_SETTINGS); // Panel Driving Settings
  ili9481_wr_dat(0x00); // установить сканирование режим
  ili9481_wr_dat(0x3B);// Установить количество линий 480
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(0x02); // 5frames
  ili9481_wr_dat(0x11);

  ili9481_wr_cmd(ILI9481_DISPLAY_TIMING_SET_FOR_NORMAL); // Display_Timing_Setting for Normal Mode
  ili9481_wr_dat(0x10);
  ili9481_wr_dat(0x0B); 
  ili9481_wr_dat(0x88);
*/

  ili9481_wr_cmd(ILI9481_FRAME_RATE_AND_INVERSION_CTRL); // Frame Rate and Inversion Control
  ili9481_wr_dat(0x00); // 125 Гц (На других частотах мельтишит)
  

  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL); // Set_address_mode 0x36
// B7         B6         B5              B4                B3        B2                B1           B0
// направл Y, направл X, выбор X или Y, Vertical направл, RGB/BGR=1, DisplDataLatch=0, Horiz Flip,  Vert Flip

  ili9481_wr_dat(0x08|LCD_ORIENTATION); // BGR-order (0x88)

  ili9481_wr_cmd(ILI9481_INTERFACE_PIXEL_FORMAT); // Set_pixel_format 0x3A
  ili9481_wr_dat(0x55); // RGB565(16dpp) 16-битный цвет


  ili9481_wr_cmd(ILI9481_DISPLAY_ON); // Display ON

}

//--------------------------------------------------
// Аналог функции usprintf сразу с выводом на LCD
// По типу print formatter (замена UARTprinf)
// Usage: ili9481_printf("1:%u 2:%u 3:%x",x,y,z);

int ili9481_printf(const char *pcString, ...)
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
  ili9481_putstr(pcBuf); // Out to LCD
  
  //
  // End the varargs processing.
  //
  va_end(vaArgP);

  //
  // Return the conversion count.
  //
  return(iRet);
}


//----------------------------------------------------------------
// Read Device Code - вывод на LCD информации о драйвере
// Для функции 0xBF (DEVICE_CODE_READ) нужно сделать непррерывно семь
// стробов чтения подряд.

void print_lcd_id (void)
{
  unsigned char tmp0,tmp1,tmp2,tmp3,tmp4,tmp5;
  ili9481_wr_cmd (ILI9481_DEVICE_CODE_READ);

  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_IN); // Шина данных на ввод

  RDX_0; // Dummy read1 (0xBF)
  RDX_1;
  
  RDX_0;
  tmp0 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0x00
  RDX_1;

  RDX_0;
  tmp1 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0x02
  RDX_1;
  
  RDX_0;
  tmp2 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0x04
  RDX_1;

  RDX_0
  tmp3 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0x94
  RDX_1;

  RDX_0;
  tmp4 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0x81
  RDX_1;

  RDX_0;
  tmp5 = GPIOPinRead(PORT_DATA, 0xFF);  // Read 0xFF
  RDX_1;

  // Шина данных опять на вывод.
  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_OUT);

  // Вывод результата
  ili9481_setpos(0,200);
  ili9481_setcolor(WHITE_COLOR, BLACK_COLOR);
  fontsize=BIG_FONT;
  
  ili9481_printf("ID0=%02x\n", tmp0);
  ili9481_printf("ID1=%02x\n", tmp1);
  ili9481_printf("ID2=%02x\n", tmp2);
  ili9481_printf("ID3=%02x\n", tmp3);
  ili9481_printf("ID4=%02x\n", tmp4);
  ili9481_printf("ID5=%02x\n", tmp5);
  
}


//--------------------------------------------------
// Установка (& подстройка) контрастности и цветопередачи
void vcom_set(unsigned char vcm, unsigned char vdv)
{
  ili9481_wr_cmd(ILI9481_VCOM_CONTROL); // VCOM Control
  ili9481_wr_dat(0x00);
  ili9481_wr_dat(vcm); // (0x3F по умолчанию)
  ili9481_wr_dat(vdv); // (0x0B по умолчанию)
}

//-----------------------------------
// Узор1
void draw_pattern1 (void)
{
  unsigned int i;
  for (i=0; i<(ILI9481_HEIGHT-1); i=i+10)
    {
      ili9481_draw(0,(ILI9481_HEIGHT-1),(ILI9481_WIDTH-1),i, RED_COLOR);
    }
  for (i=0; i<(ILI9481_HEIGHT-1); i=i+10)
    {
      ili9481_draw((ILI9481_WIDTH-1),0,0,i, YELLOW_COLOR);
    }  
}

//-----------------------------------------
// Аппаратный вертикальный скроллинг
// height - сколько скроллировать относительно исходного значения

void ili9481_vertical_scroll (unsigned int height)
{
  unsigned int i;
  for (i=0; i<height; i++)
    {
//      x=(i&0xFF);
//      if (x>210) x=0;
//      ili9481_putstr("LCD");
      
      ili9481_wr_cmd(ILI9481_VERTICAL_SCROLLING_START);
      ili9481_wr_dat(i>>8);
      ili9481_wr_dat(i);
      delay_ms(25); // Задержка, чтобы что-то увидеть
    }
}

//--------------------------------------------
// Чтение режима адресации дисплея
unsigned char ili9481_get_addr_mode(void)
{
  unsigned char tmp;
  ili9481_wr_cmd (ILI9481_READ_DISPLAY_MADCTL);

  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_IN); // Шина данных на ввод

  RDX_0; // Dummy read1
  RDX_1;
  
  RDX_0;
  tmp = GPIOPinRead(PORT_DATA, 0xFF);  // Read byte
  RDX_1;

  // Шина данных опять на вывод.
  GPIODirModeSet(PORT_DATA, 0xFF, GPIO_DIR_MODE_OUT);

  return tmp;
}

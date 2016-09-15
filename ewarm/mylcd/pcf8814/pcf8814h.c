// ======================= PCF8814 Routine ============================
// 3-Line SPI (/CS SCLK SDATA)
// При подключении сигнал /CS заведен на GND.
// ********************************************************************
// Writen by (R)soft 6.1.2014 Last Edition: 6 Jan 2014
// Version 1.0
// Tested on Cortex-M4F LM4F120H5QR
// Testing on Nokia 1200 LCD Module
// Используется плата с ЖКИ Nokia 1200 & microSD card
// Built-in PCF8814 controller with 3-line SPI interface.
// Reference manual for controller - PCF8814.pdf
// ********************************************************************
// H-version of code (High Speed)
/*
Дисплей показал хорошую работу без задержек в коде даже при
тактовой частоте контроллера 110 МГц!
*/

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
#include "font5x7.h"
#include "arial16.h"
#include "pcf8814h.h"


//===========================================
// Отправка команды в ЖКИ

void pcf8814_cmd(unsigned char cmd)
{
  SDIN_0; // Признак команды D/C=0
  SCLK_1; // Передаем первый бит
  SCLK_0; // Возврат клока в исходное состояние
  unsigned char i;
  for (i=8;i;i--)
  {
    if (cmd&0x80) // Анализируется самый старший бит в байте
      {
        SDIN_1; // и выставляется SDA в соответствии
      }
    else  
      {
        SDIN_0;
      }

    SCLK_1; // Запись по нарастанию SCLK
    SCLK_0; // Возвращаем SCLK обратно в исходное состояние

    cmd<<=1; // Сдвигаем байт влево
  }
}

//=============================================
// Отправка данных в ЖКИ

void pcf8814_data(unsigned char dat)
{
  SDIN_1; // Признак данных D/C=1
  SCLK_1; // Передаем первый бит
  SCLK_0; // Возврат клока в исходное состояние
  unsigned char i;
  for (i=8;i;i--)
  {
    if (dat&0x80) // Анализируется самый старший бит в байте
      {
        SDIN_1; // и выставляется SDA в соответствии
      }
    else  
      {
        SDIN_0;
      }

    SCLK_1; // Запись по нарастанию SCLK
    SCLK_0; // Возвращаем SCLK обратно в исходное состояние

    dat<<=1; // Сдвигаем байт влево
  }
}



//===========================================================================
// Установка адреса в ЖКИ
/*
void pcf8814_set_addr(unsigned char x, unsigned char y)
{
  if (y > (PCF8814_ROW-1)) 
    {
      y = (PCF8814_ROW-1); // Проверка максимального значения строки (0..8)
    }
  if (x > (PCF8814_PIXELX-1)) 
    {
      x = (PCF8814_PIXELX-1); // Проверка максимального значения столбца
    }
//  sed_x = x;
//  sed_y = y;
  pcf8814_cmd(PCF8814_SETYADDR | y);
  pcf8814_cmd(PCF8814_SETXADDR_LOWER | (x&0x0F));
  pcf8814_cmd(PCF8814_SETXADDR_UPPER | ((x>>4)&0x0F));
}
*/


//============================================================
// Установка координат вывода символа x,y
// Символ занимает 6 байт (с пробелом), поэтому х умножаем на 6

void pcf8814_gotoxy(unsigned char x, unsigned char y)
{
  x=(x*6);
  if (y > (PCF8814_ROW-1)) 
    {
      y = (PCF8814_ROW-1); // Проверка максимального значения строки (0..8)
    }
  if (x > (PCF8814_PIXELX-1)) 
    {
      x = (PCF8814_PIXELX-1); // Проверка максимального значения столбца
    }
  pcf8814_cmd(PCF8814_SETYADDR | y);
  pcf8814_cmd(PCF8814_SETXADDR_LOWER | (x&0x0F));
  pcf8814_cmd(PCF8814_SETXADDR_UPPER | ((x>>4)&0x0F));
}


//==========================================================================
// Вывод символа на ЖКИ в вывернутом режиме 
//(из-за неработы зеркалиирования по Х)

void pcf8814_putchar_x(unsigned char ch)
{
  unsigned int addr;

  // Вычисление адреса начала символа
  // Используется таблица символов без знаков 0x00..0x1F
  addr=(ch-0x20)*5;
  addr=addr+4;
  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr--]); 
    }
  pcf8814_data(0); // Вывод промежутка между символами
}

//==========================================================================
// Вывод символа на ЖКИ 

void pcf8814_putchar(unsigned char ch)
{
  unsigned int addr;

  // Вычисление адреса начала символа
  // Используется таблица символов без знаков 0x00..0x1F
  addr=(ch-0x20)*5;

  // Вывод пяти байт
  for (ch=0; ch<5; ch++)  
    {  
      pcf8814_data(font5x7[addr++]); 
    }
  pcf8814_data(0); // Вывод промежутка между символами
}

//==========================================================================
// Вывод символа на ЖКИ 11x16
// Ширина 11 байт, высота - 2 байта (2*8)

void pcf8814_putchar16(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char lowadd, hiadd; // Переменные вычисления х адреса
  unsigned char len; // Ширина символа в пикселах
  // Используется таблица символов без знаков 0x00..0x1F
  addr=(ch-0x20)*23; // Умножение на длину символа в байтах
  len=(arial16[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      pcf8814_cmd(PCF8814_SETYADDR|y); // Установка Y=0
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial16[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+1); // Установка Y=1
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial16[addr++]); 
    }
}


/*
//==========================================================================
// Вывод символа на ЖКИ 20x32
// Ширина 20 байт, высота - 4 байта (4*8)

void pcf8814_putchar32(unsigned char ch, unsigned char x, unsigned char y)
{
  unsigned int addr;
  unsigned char len; // Ширина символа в пикселах
  unsigned char lowadd, hiadd; // Переменные вычисления х адреса
  ch=ch-0x20; // Используется таблица символов без знаков 0x00..0x2F
  addr=ch;
  addr=addr*81; // Умножение на длину символа в байтах
  len=(arial32[addr++]); 
  for (ch=0; ch<len; ch++)  
    {  
      lowadd=(x+ch&0x0F);
      hiadd=((x+ch>>4)&0x0F);
      pcf8814_cmd(PCF8814_SETYADDR|y); // Установка Y=0
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+1); // Установка Y=1
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+2); // Установка Y=2
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
      pcf8814_cmd(PCF8814_SETYADDR|y+3); // Установка Y=3
      pcf8814_cmd(PCF8814_SETXADDR_LOWER | lowadd);
      pcf8814_cmd(PCF8814_SETXADDR_UPPER | hiadd);
      pcf8814_data(arial32[addr++]); 
    }
}
*/


//====================================
// Очистка экрана

void pcf8814_cls(void)
{
  unsigned int i;
  pcf8814_cmd(PCF8814_SETYADDR); // Установка Y=0
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); // X=0
  pcf8814_cmd(PCF8814_SETXADDR_LOWER);

  for (i=0; i<(PCF8814_PIXELX * (PCF8814_ROW+1)); i++) // Цикл для записи нулей
    {
      pcf8814_data(0); // Запись в ОЗУ LCD нулей
    }

  // После очистки экрана, стандартно - позиция вывода вверху слева
  pcf8814_cmd(PCF8814_SETYADDR); // Установка Y=0
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); // X=0
  pcf8814_cmd(PCF8814_SETXADDR_LOWER);
}

//=========================================
// Вывод строки (строка в ОЗУ)

void pcf8814_puts (unsigned char *s)
{
  while (*s) // до тех пор пока не ноль - выводим
    pcf8814_putchar(*s++);
}

/*
//=========================================
// Задать значение инверсии

void pcf8814_set_inverse(unsigned char inv)
{
//  sed_inverse=inv; // значение инверсии запоминаем в глобальной переменной
  if (inv)
    {
      pcf8814_cmd(PCF8814_DISPLAY_REVERSE);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_NORMAL);
    }
}
*/


/*
//=========================================
// Включить/выключить дисплей

void pcf8814_set_power(unsigned char power)
{
  if (power)
    {
      pcf8814_cmd(PCF8814_DISPLAY_ON);
    }
  else
    {
      pcf8814_cmd(PCF8814_DISPLAY_OFF);
    }
}
*/

//=============================================================
// Инициализация LCD

void pcf8814_init (void)
{
  // Настройка порта LCD
  SysCtlPeripheralEnable(SYS_8814);
  SysCtlGPIOAHBEnable(SYS_8814);
  GPIOPinTypeGPIOOutput(PORT_8814, PIN_RST|PIN_SDIN|PIN_SCLK);
  GPIOPadConfigSet(PORT_8814, PIN_SCLK|PIN_SDIN|PIN_RST, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  SCLK_0;

  delay_ms(50); // For Power Stable
  RST_0;  // Reset LCD
  delay_ms(50);
  RST_1;  // Return Reset OFF
//  pcf8814_cmd(PCF8814_SOFTWARE_RESET);
  delay_ms(50);
  
  pcf8814_cmd(PCF8814_DISPLAY_NORMAL);  

// Параметры для переворачивания картинки  
//  pcf8814_cmd(PCF8814_ROW_MIRROR_ON); //Зеркалирование по Х не работает
  pcf8814_cmd(PCF8814_VERTICAL_MIRROR_OFF); // Зеркалирование по Y

// BIAS 1/7 даёт большее "посинение", т.е. напряжение экрана выше
// BIAS 1/9 как бы исходное... меньше кушает тока (for VCC=2,8V)
  pcf8814_cmd(PCF8814_SET_BIAS_19); // Set BIAS

  // Подстройка контрастности дисплея - этими параметрами нужно играться до
  // победного посинения, чтобы добиться вменяемого качества картинки

  // 0x04 for VCC=2,8V
  // 0x01 - smallest, 0x06 - biggest
  pcf8814_cmd(PCF8814_RESISTOR_RATIO | 0x04); // V0 Voltage Settings 0...6 (грубо)
  // 0x1A for VCC=2,8V
  // 0x1F - biggest contrast
  pcf8814_cmd(PCF8814_ELECTRONIC_VOLUME | 0x1A); // 0...1F (точно)

  pcf8814_cmd(PCF8814_ALL_POINTS_OFF); // Нормальное отображение всех пикселей

  pcf8814_cmd(PCF8814_POWER_CTRL_SET_ON); // Включаем бустер-умножитель напряжения

  // Сдвиг начального адреса вывода (для скроллирования изображения вверх)
  pcf8814_cmd(PCF8814_START_LINE_ADDR | 0x00);

  pcf8814_cmd(PCF8814_DISPLAY_ON);
  pcf8814_cls(); // Чистим видео-ОЗУ и устанавливаем адрес вывода в начало
  
}


//====================================================================
// Выводит из буфера консоли CON на ЖКИ
// Недостаток процедуры в том, что выводится весь буфер независимо от
// заполненности. Т.е. выводятся все 128 (16*8) символов на индикатор, 
// что не всегда нужно. Но, т.к. контроллер допускает обмен по интерфейсу
// на скорости 4 МБит (т.е. без задержек), общее время вывода всей инфы
// ничтожно мало. При желании можно модифицировать и оптимизоровать
// алгоритм.
// Из-за неработы зеркалирования по Х, алгоритм вывода изменен.
// Также смотреть процедуру putchar.

void pcf8814_update_console_x(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;

  for (row=0; row<PCF8814_ROW; row++)
  {
    col=PCF8814_COL; // X
    do
      {
        col--;
        pcf8814_gotoxy(col,row);
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        pcf8814_putchar_x(c);
      }
    while (col);
   }
}




//====================================================================
// Выводит из буфера консоли CON на ЖКИ

void pcf8814_update_console(void)
{
  unsigned char col,row;
  char *p;
  char c;  
  p=CON8814;

  for (row=0; row<PCF8814_ROW; row++)
  {
  pcf8814_cmd(PCF8814_SETYADDR | row); // Установка Y
  pcf8814_cmd(PCF8814_SETXADDR_UPPER); 
  pcf8814_cmd(PCF8814_SETXADDR_LOWER); // X=0
    for (col=0; col<PCF8814_COL; col++)
      {
        c=*p++;
        if (c==0) 
          {
            c=0x20; // If End of Line (0x00) then "SPACE" character
          }
        pcf8814_putchar(c);
      }
   }
}




//==================================
// Чистим буфер консоли требуемым байтом
void pcf8814_clear_console (unsigned char byte)
{
 char i;
 char *p;
 p=CON8814;
 i=sizeof(CON8814);
 do
   {
     *p++=byte;
   }
  while (--i);
}


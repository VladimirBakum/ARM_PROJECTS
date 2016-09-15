// Version 1.0 By (R)soft 21-04-2014

//---------------------------------------------------------------------------
// Структуры BMP

//unsigned int type = bmp_file.bfh.bfType;
//unsigned int Width = bmp_file.bih.biWidth; // Ширина картинки
//unsigned int Height = bmp_file.bih.biHeight;  // Высота картинки
//  unsigned int main_offset = bmp_file.bfh.bfOffBits; // отступ от начала файла где лежат данные RGB

//  unsigned int file_size = bmp_file.bfh.bfSize;  // размер файла


#include "bmp.h"


//-------------------------------------------------
// Чтение из буера памяти, подгружаемый с microSD
unsigned char read_bufer ( void)
{

  if (bufindex>=(SDLENBUFER)) // Если указатель переполнился, подгружаем буфер
  {
    f_read(&file, &sdbufer, SDLENBUFER, &br);
    bufindex=0;
  }

return sdbufer[bufindex++];
}



//=============================== BMP Output ============================
// Вывод BMP на LCD с чтением microSD
// Координаты вывода: x, y 
// filename - имя файла с расширением bmp
// Usage: out_bmp("test.bmp");
// Return: Error Code

unsigned char out_bmp (unsigned int x, unsigned y, const char *filename)
{
  // Установка цвета и координат для вывода текста
  
  ili9481_setcolor(CYAN_COLOR, TRANSPARENT);
  ili9481_setpos(0,460);

  BMP sdbufer; // Определяем структуру BMP в буфере памяти  
  unsigned char rc; // Ошибка
  unsigned long width, height;
  unsigned long remainder;
  
//  ili9481_clear(BLACK_COLOR); // Перед выводом очищаем экран

  // Флаги чтения, записи, открытие уже имеющегося файла
  rc=f_open(&file, filename, FA_OPEN_EXISTING|FA_WRITE|FA_READ);
  
  if (rc!=FR_OK)
  {
    UARTprintf("rc=%d ERROR OPEN!!!\n", rc);
    ili9481_printf("rc=%d ERROR OPEN!!!\n", rc);
    return rc;
  }
  else UARTprintf("file %s opened!\n", filename);

  f_read(&file, &sdbufer, sizeof (BMP), &br); // Читаем заголовок файла в буфер

  //  unsigned int type = sdbufer.bfh.bfType;
  UARTprintf("BH type: %x\n", sdbufer.bfh.bfType);
  
  // Проверка на BH type (BMP)
  if (sdbufer.bfh.bfType!=0x4D42) 
  {
    f_close(&file); // Закрываем файл перед выходом
    UARTprintf("Not BMP file!\n");

    ili9481_printf("%s Not BMP File!", filename);
  
    return FR_INVALID_OBJECT; // Возвращаем ошибку
  }

  UARTprintf("File Size: %d bytes\n", sdbufer.bfh.bfSize);
  UARTprintf("Bits per pixel: %d\n", sdbufer.bih.biBitCount);

  if (sdbufer.bih.biBitCount != 24)
  {
    f_close(&file); // Закрываем файл перед выходом
    UARTprintf("NOT 24-bit picture!\n");
  
    ili9481_printf("%s Not 24-bit picture!", filename);
  
    return FR_INVALID_OBJECT; // Возвращаем ошибку        
  }
  
  width = sdbufer.bih.biWidth;
  height = sdbufer.bih.biHeight;

  // Проверка на размер картинки и экрана
  if ((width > ILI9481_WIDTH) || (height > ILI9481_HEIGHT))
  {
    f_close(&file); // Закрываем файл перед выходом
    UARTprintf("Picture Size Too Big!\n %dx%d\n", width, height);
  
    ili9481_printf("%s Picture Size Too Big! %dx%d", filename, width, height);  
  
    return FR_INVALID_OBJECT; // Возвращаем ошибку    
  }
  
  UARTprintf("width: %d\n", width);

  UARTprintf("height: %d\n", height);

  //Finding the number of bytes for padding
  remainder=(width*3)%4; // Ширина картинки, умноженная на 3 байта цвета, кратная четырем
  if(remainder!=0)    remainder = 4 - remainder;

  UARTprintf("dummy bytes: %d\n", remainder);

  unsigned char dummy;
  unsigned int w,h;
  unsigned char red, green, blue; // цвета
  unsigned int color; // 16-битный цвет для вывода

  ili9481_Window(x, y, x + (width-1), y + (height-1));  

  // Выводим картинку снизу вверх, для этого "переворачиваем" адресацию
  // столбцов и строк
  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL);
  ili9481_wr_dat(0x88|LCD_ORIENTATION);

  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  // Для ускорения вывода, кешируем чтение с карточки через буфер.
  // Индексную переменную буфера переплняем, чтобы при первом вызове
  // процедуры, буфер подгрузился.
  bufindex=SDLENBUFER+5;
           
  UARTprintf("Start output of picture... ");
  
  for (h=0; h<height; h++)  // вложенные циклы перебора столбец-строка
    {
      for (w=0; w<width; w++)
        {
          blue=read_bufer(); // чтение через кешированный буфер
          green=read_bufer();
          red=read_bufer();

          color=( ((red&0xF8)<<8) | ((green&0xFC)<<3) | (blue>>3) );

          ili9481_wr_dat((unsigned char)(color>>8)); // Установка одного пикселя
          ili9481_wr_dat((unsigned char)(color));
        }
      
      // Если остаток не равен нулю, читаем "пустые" данные для выравнивания
      if (remainder != 0)
      {
      dummy = remainder;
      while (dummy)
        {
          read_bufer(); // Read dummy byte from bufer
          dummy--;
        }
      }
      //UARTprintf(".");
    }
  
  // Возвращаем адресацию на место
  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL);
  ili9481_wr_dat(0x08|LCD_ORIENTATION);
  
  UARTprintf("End\n");

  ili9481_printf("%s %ux%u", filename, width, height);
  
  f_close(&file);
  UARTprintf("File closed.\n");
  
  return 0;
}




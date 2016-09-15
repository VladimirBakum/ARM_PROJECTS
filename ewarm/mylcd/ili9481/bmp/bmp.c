// Version 1.0 By (R)soft 21-04-2014

//---------------------------------------------------------------------------
// ��������� BMP

//unsigned int type = bmp_file.bfh.bfType;
//unsigned int Width = bmp_file.bih.biWidth; // ������ ��������
//unsigned int Height = bmp_file.bih.biHeight;  // ������ ��������
//  unsigned int main_offset = bmp_file.bfh.bfOffBits; // ������ �� ������ ����� ��� ����� ������ RGB

//  unsigned int file_size = bmp_file.bfh.bfSize;  // ������ �����


#include "bmp.h"


//-------------------------------------------------
// ������ �� ����� ������, ������������ � microSD
unsigned char read_bufer ( void)
{

  if (bufindex>=(SDLENBUFER)) // ���� ��������� ������������, ���������� �����
  {
    f_read(&file, &sdbufer, SDLENBUFER, &br);
    bufindex=0;
  }

return sdbufer[bufindex++];
}



//=============================== BMP Output ============================
// ����� BMP �� LCD � ������� microSD
// ���������� ������: x, y 
// filename - ��� ����� � ����������� bmp
// Usage: out_bmp("test.bmp");
// Return: Error Code

unsigned char out_bmp (unsigned int x, unsigned y, const char *filename)
{
  // ��������� ����� � ��������� ��� ������ ������
  
  ili9481_setcolor(CYAN_COLOR, TRANSPARENT);
  ili9481_setpos(0,460);

  BMP sdbufer; // ���������� ��������� BMP � ������ ������  
  unsigned char rc; // ������
  unsigned long width, height;
  unsigned long remainder;
  
//  ili9481_clear(BLACK_COLOR); // ����� ������� ������� �����

  // ����� ������, ������, �������� ��� ���������� �����
  rc=f_open(&file, filename, FA_OPEN_EXISTING|FA_WRITE|FA_READ);
  
  if (rc!=FR_OK)
  {
    UARTprintf("rc=%d ERROR OPEN!!!\n", rc);
    ili9481_printf("rc=%d ERROR OPEN!!!\n", rc);
    return rc;
  }
  else UARTprintf("file %s opened!\n", filename);

  f_read(&file, &sdbufer, sizeof (BMP), &br); // ������ ��������� ����� � �����

  //  unsigned int type = sdbufer.bfh.bfType;
  UARTprintf("BH type: %x\n", sdbufer.bfh.bfType);
  
  // �������� �� BH type (BMP)
  if (sdbufer.bfh.bfType!=0x4D42) 
  {
    f_close(&file); // ��������� ���� ����� �������
    UARTprintf("Not BMP file!\n");

    ili9481_printf("%s Not BMP File!", filename);
  
    return FR_INVALID_OBJECT; // ���������� ������
  }

  UARTprintf("File Size: %d bytes\n", sdbufer.bfh.bfSize);
  UARTprintf("Bits per pixel: %d\n", sdbufer.bih.biBitCount);

  if (sdbufer.bih.biBitCount != 24)
  {
    f_close(&file); // ��������� ���� ����� �������
    UARTprintf("NOT 24-bit picture!\n");
  
    ili9481_printf("%s Not 24-bit picture!", filename);
  
    return FR_INVALID_OBJECT; // ���������� ������        
  }
  
  width = sdbufer.bih.biWidth;
  height = sdbufer.bih.biHeight;

  // �������� �� ������ �������� � ������
  if ((width > ILI9481_WIDTH) || (height > ILI9481_HEIGHT))
  {
    f_close(&file); // ��������� ���� ����� �������
    UARTprintf("Picture Size Too Big!\n %dx%d\n", width, height);
  
    ili9481_printf("%s Picture Size Too Big! %dx%d", filename, width, height);  
  
    return FR_INVALID_OBJECT; // ���������� ������    
  }
  
  UARTprintf("width: %d\n", width);

  UARTprintf("height: %d\n", height);

  //Finding the number of bytes for padding
  remainder=(width*3)%4; // ������ ��������, ���������� �� 3 ����� �����, ������� �������
  if(remainder!=0)    remainder = 4 - remainder;

  UARTprintf("dummy bytes: %d\n", remainder);

  unsigned char dummy;
  unsigned int w,h;
  unsigned char red, green, blue; // �����
  unsigned int color; // 16-������ ���� ��� ������

  ili9481_Window(x, y, x + (width-1), y + (height-1));  

  // ������� �������� ����� �����, ��� ����� "��������������" ���������
  // �������� � �����
  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL);
  ili9481_wr_dat(0x88|LCD_ORIENTATION);

  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  // ��� ��������� ������, �������� ������ � �������� ����� �����.
  // ��������� ���������� ������ ����������, ����� ��� ������ ������
  // ���������, ����� �����������.
  bufindex=SDLENBUFER+5;
           
  UARTprintf("Start output of picture... ");
  
  for (h=0; h<height; h++)  // ��������� ����� �������� �������-������
    {
      for (w=0; w<width; w++)
        {
          blue=read_bufer(); // ������ ����� ������������ �����
          green=read_bufer();
          red=read_bufer();

          color=( ((red&0xF8)<<8) | ((green&0xFC)<<3) | (blue>>3) );

          ili9481_wr_dat((unsigned char)(color>>8)); // ��������� ������ �������
          ili9481_wr_dat((unsigned char)(color));
        }
      
      // ���� ������� �� ����� ����, ������ "������" ������ ��� ������������
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
  
  // ���������� ��������� �� �����
  ili9481_wr_cmd(ILI9481_MEMORY_ACCESS_CONTROL);
  ili9481_wr_dat(0x08|LCD_ORIENTATION);
  
  UARTprintf("End\n");

  ili9481_printf("%s %ux%u", filename, width, height);
  
  f_close(&file);
  UARTprintf("File closed.\n");
  
  return 0;
}




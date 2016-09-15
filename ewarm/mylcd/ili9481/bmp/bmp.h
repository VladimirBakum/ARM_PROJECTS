#ifndef _BMP_H_
#define _BMP_H_


//==========================================================================
// Структуры для BMP 
// Для IAR в сттруктурах нужно включать выравнивание до 1 байта - pack(1)!!!

#pragma pack(1) /* Выравнивание 1 байт */
typedef struct
{ 
  unsigned short bfType; // 2 bfType определяет тип файла. Здесь он должен быть BM.
  unsigned long bfSize;   // 4 размер самого файла в байтах.
  unsigned short bfReserved1; // 2 bfReserved1 и bfReserved2 зарезервированы и должны быть нулями. 
  unsigned short bfReserved2; // 2
  unsigned long bfOffBits; // 4 где начинается сам битовый массив относительно начала файла
} BITMAPFILEHEADER; // all 14 bytes


typedef struct
{
  unsigned long biSize; // 4 это размер самой структуры (Header Size)
  unsigned long biWidth; // 4 Ширина картинки в пикселах
  unsigned long biHeight; // 4 Высота картинки в пикселах
  unsigned short biPlanes; // 2 количество плоскостей. Пока оно всегда устанавливается в 1. 
  unsigned short biBitCount; // 2 Количество бит на один пиксель
  unsigned long biCompression;  // 4 обозначает тип сжатия (0..6)
  unsigned long biSizeImage; // 4 размер картинки в байтах
  unsigned long biXPelsPerMeter; // 4 горизонтальное разрешение (в пикселях на метр)
  unsigned long biYPelsPerMeter; // 4 вертикальное разрешение (в пикселях на метр)
  unsigned long biClrUsed;      // 4 количество используемых цветов из таблицы
  unsigned long biClrImportant; // 4 количество важных цветов
} BITMAPINFOHEADER; // all 40 bytes

typedef struct
{
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;
  unsigned char data_BGR[];
} BMP;
#pragma pack() /* Выравнивание по умолчанию */

/*
typedef struct
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
  unsigned char rgbReserved; 
} RGBQUAD;

typedef struct
{
  unsigned char rgbBlue; 
  unsigned char rgbGreen; 
  unsigned char rgbRed; 
} RGBTRIPLE;

#define BI_RGB            0
#define BI_RLE8           1
#define BI_RLE4           2
#define BI_BITFIELDS      3
#define BI_JPEG           4
#define BI_PNG            5
#define BI_ALPHABITFIELDS 6
*/


unsigned int br; // how many bytes of read (for f_read)

// Размер буфера увеличен до 8 кБайт из-за картинки Speccy
#define SDLENBUFER 8192 // Размер буфера microSD для кешированного чтения

static unsigned char sdbufer [SDLENBUFER];

unsigned short bufindex; // Указатель буфера

unsigned char out_bmp (unsigned int x, unsigned y, const char *filename);

#endif // _BMP_H_
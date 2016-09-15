#ifndef _BMP_H_
#define _BMP_H_


//==========================================================================
// ��������� ��� BMP 
// ��� IAR � ����������� ����� �������� ������������ �� 1 ����� - pack(1)!!!

#pragma pack(1) /* ������������ 1 ���� */
typedef struct
{ 
  unsigned short bfType; // 2 bfType ���������� ��� �����. ����� �� ������ ���� BM.
  unsigned long bfSize;   // 4 ������ ������ ����� � ������.
  unsigned short bfReserved1; // 2 bfReserved1 � bfReserved2 ��������������� � ������ ���� ������. 
  unsigned short bfReserved2; // 2
  unsigned long bfOffBits; // 4 ��� ���������� ��� ������� ������ ������������ ������ �����
} BITMAPFILEHEADER; // all 14 bytes


typedef struct
{
  unsigned long biSize; // 4 ��� ������ ����� ��������� (Header Size)
  unsigned long biWidth; // 4 ������ �������� � ��������
  unsigned long biHeight; // 4 ������ �������� � ��������
  unsigned short biPlanes; // 2 ���������� ����������. ���� ��� ������ ��������������� � 1. 
  unsigned short biBitCount; // 2 ���������� ��� �� ���� �������
  unsigned long biCompression;  // 4 ���������� ��� ������ (0..6)
  unsigned long biSizeImage; // 4 ������ �������� � ������
  unsigned long biXPelsPerMeter; // 4 �������������� ���������� (� �������� �� ����)
  unsigned long biYPelsPerMeter; // 4 ������������ ���������� (� �������� �� ����)
  unsigned long biClrUsed;      // 4 ���������� ������������ ������ �� �������
  unsigned long biClrImportant; // 4 ���������� ������ ������
} BITMAPINFOHEADER; // all 40 bytes

typedef struct
{
  BITMAPFILEHEADER bfh;
  BITMAPINFOHEADER bih;
  unsigned char data_BGR[];
} BMP;
#pragma pack() /* ������������ �� ��������� */

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

// ������ ������ �������� �� 8 ����� ��-�� �������� Speccy
#define SDLENBUFER 8192 // ������ ������ microSD ��� ������������� ������

static unsigned char sdbufer [SDLENBUFER];

unsigned short bufindex; // ��������� ������

unsigned char out_bmp (unsigned int x, unsigned y, const char *filename);

#endif // _BMP_H_
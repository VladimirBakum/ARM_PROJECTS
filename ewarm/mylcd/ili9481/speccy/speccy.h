#ifndef _SPECCY_H_
#define _SPECCY_H_

// Начальный адрес каждой строки образа экрана ZX Spectrum,
// Всего 24 строки.
static const unsigned int ZX_ADDR[] = {
  0x0000,0x0020,0x0040,0x0060,0x0080,0x00A0,0x00C0,0x00E0,
  0x0800,0x0820,0x0840,0x0860,0x0880,0x08A0,0x08C0,0x08E0,
  0x1000,0x1020,0x1040,0x1060,0x1080,0x10A0,0x10C0,0x10E0
};

// 16 bit/pixel
//-------------------------------------------------------
// R4 R3 R2 R1 | R0 G5 G4 G3 | G2 G1 G0 B4 | B3 B2 B1 B0
// 5xR:6xG:5xB

// ZX COLORS DARK      BRIGHT
// 0 BLACK   0x000000, 0x000000
// 1 BLUE    0x0000C0, 0x0000FF
// 2 RED     0xC00000, 0xFF0000
// 3 MAGENTA 0xC000C0, 0xFF00FF
// 4 GREEN   0x00C000, 0x00FF00
// 5 CYAN    0x00C0C0, 0x00FFFF
// 6 YELLOW  0xC0C000, 0xFFFF00
// 7 WHITE   0xC0C0C0, 0xFFFFFF

// Биты цвета в байте атрибутов:
// 7  6  5  4  3  2  1  0
// FL BR GP RP BP GI RI BI (FLASH, BRIGHT, GREEN PAPER, RED PAPER...)

// Таблица цветов атрибутов с яркостью и без яркости
static const unsigned int ZX_ATTR[] = {
  0x0000, // 0 BLACK
  0x0018, // 1 BLUE DARK
  0xC000, // 2 RED DARK
  0xC018, // 3 MAGENTA DARK
  0x0600, // 4 GREEN DARK
  0x0618, // 5 CYAN DARK
  0xC600, // 6 YELLOW DARK
  0xC618, // 7 WHITE DARK
  0x0000, // 0 BLACK
  0x001F, // 1 BLUE LIGHT
  0xF800, // 2 RED LIGHT
  0xF81F, // 3 MAGENTA LIGHT
  0x07E0, // 4 GREEN LIGHT
  0x07FF, // 5 CYAN LIGHT
  0xFFE0, // 6 YELLOW LIGHT
  0xFFFF  // 7 WHITE LIGHT
};


//-------------------------------------------------
// Func declarations

void DrawScreen2_zx(const unsigned char *pScreen);


#endif //_SPECCY_H_


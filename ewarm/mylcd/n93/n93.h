#ifndef __N93_H__
#define __N93_H__

// Для упрощения работы с кодом, префикс контроллера MC2PA8201
// заменен на более короткое N93 (модель телефона).
// Код также подходит для дисплеев от Nokia N71, N73.
//-------------------------
// MC2PA8201 частично совместим с R69406, не проверено (нет даташита).
// MC2PA8201 по регистрам совместим с LDS285
// MC2PA8201(Nokia6300 & NokiaE51)
//--------------------------
// MC2PA8201 Header for ARM (LM4F120)
// By (R)soft 12.11.2013-24.1.2014
// Connecting LCD: PORTx - Data, PORTx - Control/debug

#define PORT_CTRL       GPIO_PORTE_AHB_BASE
#define SYS_CTRL        SYSCTL_PERIPH_GPIOE

#define PORT_DATA       GPIO_PORTD_AHB_BASE
#define SYS_DATA        SYSCTL_PERIPH_GPIOD

#define PIN_DCX   GPIO_PIN_0 // DATA Command Bit: 0 - Command, 1 - Data
#define PIN_RDX   GPIO_PIN_1 // Read Data (Active is LOW LEVEL)
#define PIN_WRX   GPIO_PIN_2 // Write Data (Active is LOW LEVEL)
#define PIN_RESX  GPIO_PIN_3  // /Reset Chip (Active is LOW LEVEL)

// Определения вкл/выкл ног
#define DCX_1   HWREG(PORT_CTRL | (PIN_DCX << 2)) = PIN_DCX;
#define DCX_0   HWREG(PORT_CTRL | (PIN_DCX << 2)) = 0;
#define WRX_1   HWREG(PORT_CTRL | (PIN_WRX << 2)) = PIN_WRX;
#define WRX_0   HWREG(PORT_CTRL | (PIN_WRX << 2)) = 0;
#define RDX_1  	HWREG(PORT_CTRL | (PIN_RDX << 2)) = PIN_RDX;
#define RDX_0  	HWREG(PORT_CTRL | (PIN_RDX << 2)) = 0;
#define RESX_1  HWREG(PORT_CTRL | (PIN_RESX << 2)) = PIN_RESX;
#define RESX_0  HWREG(PORT_CTRL | (PIN_RESX << 2)) = 0;

//-------------------------------------------------------------------------
// PIXELY=320, PIXELX=240 (портретная)
//#define LCD_ORIENTATION 0x00      //VERTICAL_NORMAL - main2
//#define LCD_ORIENTATION 0x40    //VERTICAL_MIRROR_X
//#define LCD_ORIENTATION 0x80    //VERTICAL_MIRROR_Y - main3
//#define LCD_ORIENTATION 0xC0    //VERTICAL_MIRROR_XY - main

// PIXELY=240, PIXELX=320 (альбомная)
//#define LCD_ORIENTATION 0x20    //HORIZONTAL_NORMAL
//#define LCD_ORIENTATION 0x60    //HORIZONTAL_MIRROR_X - ok
#define LCD_ORIENTATION 0xA0    //HORIZONTAL_MIRROR_Y - ok
//#define LCD_ORIENTATION 0xE0    //HORIZONTAL_MIRROR_XY


// Размеры экрана 320x240
// В зависимости от портретной или альбомной ориентации.
// смотреть параметр LCD_ORIENTATION и менять значения

#define N93_PIXELY  (240)
#define N93_PIXELX  (320)


#define N93_WIDTH   N93_PIXELX  // Ширина
#define N93_HEIGHT  N93_PIXELY  // Высота

#define N93_ROW     (N93_PIXELY/8) // Количество строк
#define N93_COL     (N93_PIXELX/8) // Количество символов в строке


#define SMALL_FONT      0
#define MEDIUM_FONT     1
#define BIG_FONT        2


// Команды ЖКИ
#define N93_NOP                               0x00
#define N93_SOFTWARE_RESET                    0x01
#define N93_READ_DISPLAY_ID                   0x04
#define N93_READ_DISPLAY_STATUS               0x09
#define N93_READ_DISPLAY_POWER_MODE           0x0A
#define N93_READ_DISPLAY_MADCTL               0x0B
#define N93_READ_DISPLAY_PIXEL_FORMAT         0x0C
#define N93_READ_DISPLAY_IMAGE_MODE           0x0D
#define N93_READ_DISPLAY_SIGNAL_MODE          0x0E
#define N93_READ_DISPLAY_SELF_DIAGNOSTICS     0x0F
#define N93_SLEEP_IN                          0x10
#define N93_SLEEP_OUT                         0x11
#define N93_PARTIAL_MODE_ON                   0x12
#define N93_NORMAL_DISPLAY_MODE_ON            0x13
#define N93_DISPLAY_INVERSION_OFF             0x20
#define N93_DISPLAY_INVERSION_ON              0x21
#define N93_GAMMA_SET                         0x26
#define N93_DISPLAY_OFF                       0x28
#define N93_DISPLAY_ON                        0x29
#define N93_COLUMN_ADDRESS_SET                0x2A
#define N93_PAGE_ADDRESS_SET                  0x2B
#define N93_MEMORY_WRITE                      0x2C
#define N93_COLOUR_SET                        0x2D
#define N93_MEMORY_READ                       0x2E
#define N93_PARTIAL_AREA                      0x30
#define N93_VERTICAL_SCROLLING_DEFINITION     0x33
#define N93_TEARING_EFFECT_LINE_OFF           0x34
#define N93_TEARING_EFFECT_LINE_ON            0x35
#define N93_MEMORY_ACCESS_CONTROL             0x36
#define N93_VERTICAL_SCROLLING_START_ADDRESS  0x37
#define N93_IDLE_MODE_OFF                     0x38
#define N93_IDLE_MODE_ON                      0x39
#define N93_INTERFACE_PIXEL_FORMAT            0x3A
#define N93_READ_ID1                          0xDA
#define N93_READ_ID2                          0xDB
#define N93_READ_ID3                          0xDC

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


// 30 символов в строке, всего 40 строк текста (шрифт 8х8)
//char CON_N93 [N93_COL*N93_ROW+1]; // CONsole buffer для вывода функцией sprintf

// Определения начала каждой строки в консоли
#define LINE_01 CON_N93
#define LINE_02 CON_N93+N93_COL
#define LINE_03 CON_N93+(N93_COL*2)
#define LINE_04 CON_N93+(N93_COL*3)

// 24-bit colors
#define COLOR_RED       0xFF0000
#define DEEPPINK  0xFF1493
#define ORANGE    0xFFA500
#define YELLOW    0xFFFF00
#define GOLD      0xFFD700
#define VIOLET    0xEE82EE
#define MAGENTA   0xFF00FF
#define BLUEVIOLET  0x8A2BE2
#define PURPLE    0x800080
#define LIME      0x00FF00
#define COLOR_GREEN     0x008000
#define OLIVE     0x808000
#define TEAL      0x008080
#define CYAN      0x00FFFF
#define SKYBLUE   0x87CEEB
#define COLOR_BLUE      0x0000FF
#define DARKBLUE  0x00008B
#define NAVY      0x000080
#define CHOCOLATE 0xD2691E
#define BROWN     0xA52A2A
#define WHITE     0xFFFFFF
#define DARKGREY  0xA9A9A9
#define GRAY      0x808080
#define BLACK     0x000000


// 16-bit colors
#define TRANSPARENT     0xA5A5  // Специальный код прозрачного цвета
#define BLACK_COLOR     0x0000
#define NAVY_COLOR      0x000F
#define BLUE_COLOR      0x001F
#define GREEN_COLOR     0x07E0
#define CYAN_COLOR      0x07FF
#define PURPLE_COLOR    0x780F
#define OLIVE_COLOR     0x7BE0
#define RED_COLOR       0xF800
#define MAGENTA_COLOR   0xF81F
#define ORANGE_COLOR    0xFD20
#define YELLOW_COLOR    0xFFE0
#define WHITE_COLOR     0xFFFF
// 16 bit/pixel
//-------------------------------------------------------
// R4 R3 R2 R1 | R0 G5 G4 G3 | G2 G1 G0 B4 | B3 B2 B1 B0
// 5xR:6xG:5xB

/*
// some RGB color definitions                                                 
#define Black           0x0000      //   0,   0,   0 
#define Navy            0x000F      //   0,   0, 128 
#define DarkGreen       0x03E0      //   0, 128,   0 
#define DarkCyan        0x03EF      //   0, 128, 128 
#define Maroon          0x7800      // 128,   0,   0 
#define Purple          0x780F      // 128,   0, 128 
#define Olive           0x7BE0      // 128, 128,   0 
#define LightGrey       0xC618      // 192, 192, 192 
#define DarkGrey        0x7BEF      // 128, 128, 128 
#define Blue            0x001F      //   0,   0, 255 
#define Green           0x07E0      //   0, 255,   0 
#define Cyan            0x07FF      //   0, 255, 255 
#define Red             0xF800      // 255,   0,   0 
#define Magenta         0xF81F      // 255,   0, 255 
#define Yellow          0xFFE0      // 255, 255,   0 
#define White           0xFFFF      // 255, 255, 255 
#define Orange          0xFD20      // 255, 165,   0 
#define GreenYellow     0xAFE5      // 173, 255,  47 
#define Pink            0xF81F
*/

#endif
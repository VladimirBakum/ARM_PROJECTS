//===============================================
// ILI9481 Header for TFT8K2346 & TFT8K5029 LCMs
// ARM Cortex M4F Verion (LM4F120H5QR)
// 8-bit Bus version (IM0=1 & IM1=1)
// By (R)soft 20-04-2014 ... 01-05-2014
// Version 1.1
//===============================================

#ifndef __ILI9481_8BIT_H__
#define __ILI9481_8BIT_H__

// Port of Control Bus
#define PORT_CTRL       GPIO_PORTB_AHB_BASE // был GPIO_PORTB_AHB_BASE
#define SYS_CTRL        SYSCTL_PERIPH_GPIOB

// Port of Data Bus
#define PORT_DATA       GPIO_PORTD_AHB_BASE // был GPIO_PORTB_AHB_BASE
#define SYS_DATA        SYSCTL_PERIPH_GPIOD

#define PIN_RESX  GPIO_PIN_0    // Reset Chip (Active is LOW LEVEL)
#define PIN_RDX   GPIO_PIN_1    // Read Data (Active is LOW LEVEL)
#define PIN_WRX   GPIO_PIN_2    // Write Data (Active is LOW LEVEL)
#define PIN_DCX   GPIO_PIN_3    // DATA Command Bit: 0 - Command, 1 - Data

// Определения вкл/выкл ног
#define DCX_1   HWREG(PORT_CTRL + (PIN_DCX << 2)) = PIN_DCX;
#define DCX_0   HWREG(PORT_CTRL + (PIN_DCX << 2)) = 0;
#define WRX_1   HWREG(PORT_CTRL + (PIN_WRX << 2)) = PIN_WRX;
#define WRX_0   HWREG(PORT_CTRL + (PIN_WRX << 2)) = 0;
#define RDX_1  	HWREG(PORT_CTRL + (PIN_RDX << 2)) = PIN_RDX;
#define RDX_0  	HWREG(PORT_CTRL + (PIN_RDX << 2)) = 0;
#define RESX_1  HWREG(PORT_CTRL + (PIN_RESX << 2)) = PIN_RESX;
#define RESX_0  HWREG(PORT_CTRL + (PIN_RESX << 2)) = 0;

// LCD Size 480x272
// Первые 24 точки по Y не видны, поэтому при выводе это надо учесть

#define ILI9481_PIXELY          (272) // 272 for VERTICAL
#define ILI9481_PIXELX          (480) // 480 for HORIZONTAL
#define ILI9481_UNVISIBLE       (24) // Невидимые столбцы в начале памяти

//-------------------------------------------------------
// Здесь задается портретная или альбомная ориентация

#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT  // PORTRAIT or PORTRAIT_FLIP or LANDSCAPE or LANDSCAPE_FLIP
#endif

#ifdef PORTRAIT
#define LCD_ORIENTATION 0x01
#define ILI9481_WIDTH     ILI9481_PIXELY  // Ширина
#define ILI9481_HEIGHT    ILI9481_PIXELX  // Высота
#endif
#ifdef LANDSCAPE
#define LCD_ORIENTATION 0x23
#define ILI9481_WIDTH     ILI9481_PIXELX
#define ILI9481_HEIGHT    ILI9481_PIXELY
#endif
#ifdef PORTRAIT_FLIP
#define LCD_ORIENTATION 0x02
#define ILI9481_WIDTH     ILI9481_PIXELY
#define ILI9481_HEIGHT    ILI9481_PIXELX
#endif
#ifdef LANDSCAPE_FLIP
#define LCD_ORIENTATION 0x20
#define ILI9481_WIDTH     ILI9481_PIXELX
#define ILI9481_HEIGHT    ILI9481_PIXELY
#endif


// ============== Command Set ======================
#define ILI9481_NOP                               0x00
#define ILI9481_SOFTWARE_RESET                    0x01
#define ILI9481_READ_DISPLAY_ID                   0x04
#define ILI9481_READ_DISPLAY_STATUS               0x09
#define ILI9481_READ_DISPLAY_POWER_MODE           0x0A
#define ILI9481_READ_DISPLAY_MADCTL               0x0B
#define ILI9481_READ_DISPLAY_PIXEL_FORMAT         0x0C
#define ILI9481_READ_DISPLAY_IMAGE_MODE           0x0D
#define ILI9481_READ_DISPLAY_SIGNAL_MODE          0x0E
#define ILI9481_READ_DISPLAY_SELF_DIAGNOSTICS     0x0F
#define ILI9481_SLEEP_IN                          0x10
#define ILI9481_SLEEP_OUT                         0x11
#define ILI9481_PARTIAL_MODE_ON                   0x12
#define ILI9481_NORMAL_DISPLAY_MODE_ON            0x13
#define ILI9481_DISPLAY_INVERSION_OFF             0x20
#define ILI9481_DISPLAY_INVERSION_ON              0x21
#define ILI9481_GAMMA_SET                         0x26
#define ILI9481_DISPLAY_OFF                       0x28
#define ILI9481_DISPLAY_ON                        0x29
#define ILI9481_COLUMN_ADDRESS_SET                0x2A
#define ILI9481_PAGE_ADDRESS_SET                  0x2B
#define ILI9481_MEMORY_WRITE                      0x2C
#define ILI9481_COLOUR_SET                        0x2D
#define ILI9481_MEMORY_READ                       0x2E
#define ILI9481_PARTIAL_AREA                      0x30
#define ILI9481_VERTICAL_SCROLLING_DEFINITION     0x33
#define ILI9481_TEARING_EFFECT_LINE_OFF           0x34
#define ILI9481_TEARING_EFFECT_LINE_ON            0x35
#define ILI9481_MEMORY_ACCESS_CONTROL             0x36
#define ILI9481_VERTICAL_SCROLLING_START          0x37
#define ILI9481_IDLE_MODE_OFF                     0x38
#define ILI9481_IDLE_MODE_ON                      0x39
#define ILI9481_INTERFACE_PIXEL_FORMAT            0x3A
#define ILI9481_WRITE_MEMORY_CONTINUE             0x3C
#define ILI9481_READ_MEMORY_CONTINUE              0x3E
#define ILI9481_SET_TEAR_SCANLINE                 0x44
#define ILI9481_GET_SCANLINE                      0x45
#define ILI9481_READ_DDB_START                    0xA1
#define ILI9481_COMMAND_ACCESS_PROTECT            0xB0
#define ILI9481_LOW_POWER_MODE_CONTROL            0xB1
#define ILI9481_FRAME_MEM_ACCESS_AND_INTERF_SET   0xB3
#define ILI9481_DISPL_MODE_AND_FRAME_MEM_SET      0xB4
#define ILI9481_DEVICE_CODE_READ                  0xBF
#define ILI9481_PANEL_DRIVING_SETTINGS            0xC0
#define ILI9481_DISPLAY_TIMING_SET_FOR_NORMAL     0xC1
#define ILI9481_DISPLAY_TIMING_SET_FOR_PARTIAL    0xC2
#define ILI9481_DISPLAY_TIMING_SET_FOR_IDLE       0xC3
#define ILI9481_FRAME_RATE_AND_INVERSION_CTRL     0xC5
#define ILI9481_INTERFACE_CONTROL                 0xC6
#define ILI9481_GAMMA_SETTING                     0xC8
#define ILI9481_POWER_SETTING                     0xD0
#define ILI9481_VCOM_CONTROL                      0xD1
#define ILI9481_POWER_SET_FOR_NOMAL_MODE          0xD2
#define ILI9481_POWER_SET_FOR_PARTIAL_MODE        0xD3
#define ILI9481_POWER_SET_FOR_IDLE_MODE           0xD4

//---------------------------------------------------------------
// 16-bit colors
#define TRANSPARENT     0xA5A5  // Специальный код прозрачного цвета
#define PURPLE_COLOR    0x780F //
#define CYAN_COLOR      0x07FF//
#define YELLOW_COLOR    0xFFE0//
#define WHITE_COLOR     0xFFFF
#define BLUE_COLOR      0x001F  //
#define NAVY_COLOR      0x000F
#define GREEN_COLOR     0x07E0//
#define RED_COLOR       0xF800//
#define BLACK_COLOR     0x0000//
#define OLIVE_COLOR     0x7BE0 //
#define ORANGE_COLOR    0xFD20//
#define PINK_COLOR      0xF81F//

#define SMALL_FONT      0
#define MEDIUM_FONT     1
#define BIG_FONT        2

//--------------------------------
// Colors - цвета вывода на LCD
unsigned short fColor; // foreground color
unsigned short bColor; // background color

//--------------------------------------------------------------
// Переменные для запоминания текущих координат вывода текста
unsigned short x=0;
unsigned short y=0;

// Размер шрифта (0,1,2)
unsigned char fontsize=0;

char BUFER [128]; // Буфер печати символов
      
/*
//------------------------------------------------------------
// 24-bit colors
#define RED       0xFF0000
#define DEEPPINK  0xFF1493
#define ORANGE    0xFFA500
#define YELLOW    0xFFFF00
#define GOLD      0xFFD700
#define VIOLET    0xEE82EE
#define MAGENTA   0xFF00FF
#define BLUEVIOLET  0x8A2BE2
#define PURPLE    0x800080
#define LIME      0x00FF00
#define GREEN     0x008000
#define OLIVE     0x808000
#define TEAL      0x008080
#define CYAN      0x00FFFF
#define SKYBLUE   0x87CEEB
#define BLUE      0x0000FF
#define DARKBLUE  0x00008B
#define NAVY      0x000080
#define CHOCOLATE 0xD2691E
#define BROWN     0xA52A2A
#define WHITE     0xFFFFFF
#define DARKGREY  0xA9A9A9
#define GRAY      0x808080
#define BLACK     0x000000
*/

#endif  // __ILI9481_8BIT_H__
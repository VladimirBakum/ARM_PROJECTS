//===============================================
// SSD1963 Header for ITDB02-4.3 LCM
// ARM Cortex M4F Verion (LM4F120H5QR)
// 8-bit Bus version
// By (R)soft 06-03-2015 ...
// Version 1.0
//===============================================

#ifndef __SSD1963_8BIT_H__
#define __SSD1963_8BIT_H__

// Port of Control Bus
#define PORT_CTRL       GPIO_PORTB_AHB_BASE
#define SYS_CTRL        SYSCTL_PERIPH_GPIOB

// Port of Data Bus
#define PORT_DATA       GPIO_PORTD_AHB_BASE
#define SYS_DATA        SYSCTL_PERIPH_GPIOD


#define PIN_RDX         GPIO_PIN_1    // Read Data (Active is LOW LEVEL)
#define PIN_WRX         GPIO_PIN_2    // Write Data (Active is LOW LEVEL)
#define PIN_DCX         GPIO_PIN_3    // DATA Command Bit: 0 - Command, 1 - Data
#define PIN_RESETX      GPIO_PIN_4    // Reset Chip (Active is LOW LEVEL)
#define PIN_CSX         GPIO_PIN_5    // Chip Select (Active is LOW LEVEL)

// Определения вкл/выкл ног
#define DCX_1   HWREG(PORT_CTRL + (PIN_DCX << 2)) = PIN_DCX;
#define DCX_0   HWREG(PORT_CTRL + (PIN_DCX << 2)) = 0;
#define WRX_1   HWREG(PORT_CTRL + (PIN_WRX << 2)) = PIN_WRX;
#define WRX_0   HWREG(PORT_CTRL + (PIN_WRX << 2)) = 0;
#define RDX_1  	HWREG(PORT_CTRL + (PIN_RDX << 2)) = PIN_RDX;
#define RDX_0   HWREG(PORT_CTRL + (PIN_RDX << 2)) = 0;
#define RESETX_1  HWREG(PORT_CTRL + (PIN_RESETX << 2)) = PIN_RESETX;
#define RESETX_0  HWREG(PORT_CTRL + (PIN_RESETX << 2)) = 0;
#define CSX_1   HWREG(PORT_CTRL + (PIN_CSX << 2)) = PIN_CSX;
#define CSX_0   HWREG(PORT_CTRL + (PIN_CSX << 2)) = 0;

// LCD Size 480x272

#define SSD1963_PIXELY          (272) // 272 for VERTICAL
#define SSD1963_PIXELX          (480) // 480 for HORIZONTAL

//-------------------------------------------------------
// Здесь задается портретная или альбомная ориентация

#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define PORTRAIT  // PORTRAIT or PORTRAIT_FLIP or LANDSCAPE or LANDSCAPE_FLIP
#endif

#ifdef PORTRAIT
#define LCD_ORIENTATION 0x01
#define SSD1963_WIDTH     SSD1963_PIXELY  // Ширина
#define SSD1963_HEIGHT    SSD1963_PIXELX  // Высота
#endif
#ifdef LANDSCAPE
#define LCD_ORIENTATION 0x23
#define SSD1963_WIDTH     SSD1963_PIXELX
#define SSD1963_HEIGHT    SSD1963_PIXELY
#endif
#ifdef PORTRAIT_FLIP
#define LCD_ORIENTATION 0x02
#define SSD1963_WIDTH     SSD1963_PIXELY
#define SSD1963_HEIGHT    SSD1963_PIXELX
#endif
#ifdef LANDSCAPE_FLIP
#define LCD_ORIENTATION 0x20
#define SSD1963_WIDTH     SSD1963_PIXELX
#define SSD1963_HEIGHT    SSD1963_PIXELY
#endif


// ============== Command Set ======================
#define SSD1963_NOP                             0x00
#define SSD1963_SOFTWARE_RESET                  0x01
#define SSD1963_GET_POWER_MODE                  0x0A
#define SSD1963_GET_ADDRESS_MODE                0x0B
#define SSD1963_GET_PIXEL_FORMAT                0x0C
#define SSD1963_GET_DISPLAY_MODE                0x0D
#define SSD1963_GET_SIGNAL_MODE                 0x0E
#define SSD1963_SLEEP_IN                        0x10
#define SSD1963_SLEEP_OUT                       0x11
#define SSD1963_PARTIAL_MODE_ON                 0x12
#define SSD1963_NORMAL_DISPLAY_MODE_ON          0x13
#define SSD1963_DISPLAY_INVERSION_OFF           0x20
#define SSD1963_DISPLAY_INVERSION_ON            0x21
#define SSD1963_GAMMA_SET                       0x26
#define SSD1963_DISPLAY_OFF                     0x28
#define SSD1963_DISPLAY_ON                      0x29
#define SSD1963_COLUMN_ADDRESS_SET              0x2A
#define SSD1963_PAGE_ADDRESS_SET                0x2B
#define SSD1963_MEMORY_WRITE                    0x2C
#define SSD1963_MEMORY_READ                     0x2E
#define SSD1963_PARTIAL_AREA                    0x30
#define SSD1963_VERTICAL_SCROLLING_AREA         0x33
#define SSD1963_TEARING_EFFECT_LINE_OFF         0x34
#define SSD1963_TEARING_EFFECT_LINE_ON          0x35
#define SSD1963_SET_ADDRESS_MODE                0x36
#define SSD1963_VERTICAL_SCROLLING_START        0x37
#define SSD1963_IDLE_MODE_OFF                   0x38
#define SSD1963_IDLE_MODE_ON                    0x39
#define SSD1963_SET_PIXEL_FORMAT                0x3A
#define SSD1963_WRITE_MEMORY_CONTINUE           0x3C
#define SSD1963_READ_MEMORY_CONTINUE            0x3E
#define SSD1963_SET_TEAR_SCANLINE               0x44
#define SSD1963_GET_SCANLINE                    0x45
#define SSD1963_READ_DDB_START                  0xA1
#define SSD1963_SET_LCD_MODE                    0xB0
#define SSD1963_GET_LCD_MODE                    0xB1
#define SSD1963_SET_HORI_PERIOD                 0xB4
#define SSD1963_GET_HORI_PERIOD                 0xB5
#define SSD1963_SET_VERT_PERIOD                 0xB6
#define SSD1963_GET_VERT_PERIOD                 0xB7
#define SSD1963_SET_GPIO_CONF                   0xB8
#define SSD1963_GET_GPIO_CONF                   0xB9
#define SSD1963_SET_GPIO_VALUE                  0xBA
#define SSD1963_GET_GPIO_STATUS                 0xBB
#define SSD1963_SET_POST_PROC                   0xBC
#define SSD1963_GET_POST_PROC                   0xBD
#define SSD1963_SET_PWM_CONF                    0xBE
#define SSD1963_GET_PWM_CONF                    0xBF
#define SSD1963_SET_LCD_GEN0                    0xC0
#define SSD1963_GET_LCD_GEN0                    0xC1
#define SSD1963_SET_LCD_GEN1                    0xC2
#define SSD1963_GET_LCD_GEN1                    0xC3
#define SSD1963_SET_LCD_GEN2                    0xC4
#define SSD1963_GET_LCD_GEN2                    0xC5
#define SSD1963_SET_LCD_GEN3                    0xC6
#define SSD1963_GET_LCD_GEN3                    0xC7
#define SSD1963_SET_GPIO0_ROP                   0xC8
#define SSD1963_GET_GPIO0_ROP                   0xC9
#define SSD1963_SET_GPIO1_ROP                   0xCA
#define SSD1963_GET_GPIO1_ROP                   0xCB
#define SSD1963_SET_GPIO2_ROP                   0xCC
#define SSD1963_GET_GPIO2_ROP                   0xCD
#define SSD1963_SET_GPIO3_ROP                   0xCE
#define SSD1963_GET_GPIO3_ROP                   0xCF
#define SSD1963_SET_DBC_CONF                    0xD0
#define SSD1963_GET_DBC_CONF                    0xD1
#define SSD1963_SET_DBC_TH                      0xD4
#define SSD1963_GET_DBC_TH                      0xD5
#define SSD1963_SET_PLL                         0xE0
#define SSD1963_SET_PLL_MN                      0xE2
#define SSD1963_GET_PLL_MN                      0xE3
#define SSD1963_GET_PLL_STATUS                  0xE4
#define SSD1963_SET_DEEP_SLEEP                  0xE5
#define SSD1963_SET_LSHIFT_FREQ                 0xE6
#define SSD1963_GET_LSHIFT_FREQ                 0xE7
#define SSD1963_SET_PIXEL_DATA_INTERFACE        0xF0
#define SSD1963_GET_PIXEL_DATA_INTERFACE        0xF1

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

#endif  // __SSD1963_8BIT_H__
#ifndef __PCF8814_H__
#define __PCF8814_H__


//----------------------------------------------------
// ARM Realization by (R)soft 6.1.2014
// Команды pcf8814 совместимы с STE2007
// Используется плата с ЖКИ Nokia 1200 & microSD card
//----------------------------------------------------
// H-version of code (High Speed)

// За какие лапки будем дергать LCD
#define PORT_8814       GPIO_PORTE_AHB_BASE
#define SYS_8814        SYSCTL_PERIPH_GPIOE
#define PIN_SCLK    GPIO_PIN_6 // Clock Data
#define PIN_SDIN    GPIO_PIN_7 // Serial Data Input
#define PIN_RST     GPIO_PIN_5 // Reset LCD

#define SCLK_1  HWREG(PORT_8814 | (PIN_SCLK << 2)) = PIN_SCLK;
#define SCLK_0  HWREG(PORT_8814 | (PIN_SCLK << 2)) = 0;
#define SDIN_1  HWREG(PORT_8814 | (PIN_SDIN << 2)) = PIN_SDIN;
#define SDIN_0  HWREG(PORT_8814 | (PIN_SDIN << 2)) = 0;
#define RST_1   HWREG(PORT_8814 | (PIN_RST << 2)) = PIN_RST;
#define RST_0   HWREG(PORT_8814 | (PIN_RST << 2)) = 0;

// Размеры экрана
#define PCF8814_PIXELX  (96)
#define PCF8814_PIXELY  (65)
#define PCF8814_ROW     (8) // Количество строк
#define PCF8814_COL     (16) // Количество символов в строке

// Команды ЖКИ

#define PCF8814_SET_BIAS_19         (0xA2) // Set BIAS 1/9
#define PCF8814_SET_BIAS_17         (0xA3) // Set BIAS 1/7

#define PCF8814_ALL_POINTS_OFF      (0xA4) // Нормальное отображение
#define PCF8814_ALL_POINTS_ON       (0xA5) // Включить все пиксели

#define PCF8814_DISPLAY_NORMAL      (0xA6)
#define PCF8814_DISPLAY_REVERSE     (0xA7)

#define PCF8814_DATA_ORDER_OFF      (0xA8)
#define PCF8814_DATA_ORDER_ON       (0xA9)

#define PCF8814_RAM_ADDR_MODE_MSB   (0xAA)
#define PCF8814_RAM_ADDR_MODE_LSB   (0xAB)

#define PCF8814_PARTIAL_POSITION    (0xAC) // And second byte - 0...7

#define PCF8814_NLINE_INVERSION     (0xAD)  // And second byte - Value

#define PCF8814_DISPLAY_OFF         (0xAE) // Display OFF для снижения энергопотребления
#define PCF8814_DISPLAY_ON          (0xAF)

#define PCF8814_VERTICAL_MIRROR_OFF (0xC0)
#define PCF8814_VERTICAL_MIRROR_ON  (0xC8)

#define PCF8814_SET_PARTIAL_DISPLAY (0xD0) // 0...7

#define PCF8814_ROW_MIRROR_OFF      (0xE0)
#define PCF8814_ROW_MIRROR_ON       (0xE1)

#define PCF8814_SOFTWARE_RESET      (0xE2)

#define PCF8814_RESISTOR_RATIO      (0x20) // 0...6
#define PCF8814_ELECTRONIC_VOLUME   (0x80) // | 0...1F (1F - low, 0 - high)

#define PCF8814_POWER_CTRL_SET_OFF  (0x28)
#define PCF8814_POWER_CTRL_SET_ON   (0x2F) // ??? 0x2C не пашет
#define PCF8814_POWER_CTRL_SET      (0x2C)

#define PCF8814_START_LINE_ADDR    (0x40) // С какого адреса выводить 0...3F

// 0 <= Y <= 8
#define PCF8814_SETYADDR           (0xB0) // Установка адреса строки - последние 4 бита 0...8

// 0 <= X <= 131
#define PCF8814_SETXADDR_LOWER     (0x00) // Set A3...A0 (Low Tetrade)
#define PCF8814_SETXADDR_UPPER     (0x10) // Set A7...A4 (High Tetrade)

// Bias Defines
#define PCF8814_BIAS_100  (0x00) // 1:100
#define PCF8814_BIAS_80   (0x01) // 1:80
#define PCF8814_BIAS_65   (0x02)
#define PCF8814_BIAS_48   (0x03)
#define PCF8814_BIAS_40   (0x04)
#define PCF8814_BIAS_24   (0x05)
#define PCF8814_BIAS_18   (0x06)
#define PCF8814_BIAS_10   (0x07)

// Основные глобальные (volatile) переменные LCD
//volatile unsigned char lcd8814_x = 0; // Позиция X, которая указывается при выводе текста
//volatile unsigned char lcd8814_y = 0;
//volatile unsigned char lcd8814_contrast = 0; // Значение контрастности
//volatile unsigned char lcd8814_inverse = 0; // Инверсия вывода (0 - выключена)



// 16 символов в строке, всего 8 строк текста
char CON8814 [PCF8814_COL*PCF8814_ROW+1]; // CONsole buffer для вывода функцией sprintf (128 символов)

// Определения начала каждой строки в консоли
#define LN1 CON8814
#define LN2 CON8814+PCF8814_COL
#define LN3 CON8814+(PCF8814_COL*2)
#define LN4 CON8814+(PCF8814_COL*3)
#define LN5 CON8814+(PCF8814_COL*4)
#define LN6 CON8814+(PCF8814_COL*5)
#define LN7 CON8814+(PCF8814_COL*6)
#define LN8 CON8814+(PCF8814_COL*7)



// function prototypes
void delay_ms (unsigned int value);
void delay_us (unsigned int value);
void pcf8814_write (unsigned char ch); // Запись байта в ЖКИ
void pcf8814_cmd(unsigned char cmd); // Отправка команды в ЖКИ
void pcf8814_data(unsigned char dat); // Отправка данных в ЖКИ
void pcf8814_set_addr(unsigned char x, unsigned char y); // Установка адреса в ЖКИ
void pcf8814_gotoxy(unsigned char x, unsigned char y); // Установка позиции вывода по координатам x,y
void pcf8814_putchar(unsigned char ch); // Вывод символа на ЖКИ
void pcf8814_cls(void); // Очистка экрана - заполнение ОЗУ нулями, установка позиции вывода в ноль
void pcf8814_puts (unsigned char *s); // Вывод строки (строка в ОЗУ)
void pcf8814_set_contrast(unsigned char c); // Задать значение контраста
void pcf8814_set_inverse(unsigned char inv); // Задать значение инверсии
void pcf8814_set_power(unsigned char power); // Включить/выключить дисплей
void pcf8814_init (void); // Инициализания контроллера AVR
void pcf8814_update_console(void); // Выводит из буфера консоли CON на ЖКИ
void pcf8814_clear_console (unsigned char byte); // Заполнение буфера консоли CON требуемым байтом

#endif

/***********************************
*
* www.fatihinanc.com
* 
* Muhammed Fatih İNANÇ
* 09.12.2012
*
* LM4F120 için 2x16 LCD Kütüphanesi
* Stellaris LaunchPad
*
***********************************/
/*

RS -> PB0
EN -> PB1

D4 -> PD0
D5 -> PD1
D6 -> PD2
D7 -> PD3

*/

#define delay_us(x)     systick_delay_us(x)
#define delay_ms(x)     systick_delay_ms(x)

#define LCD_DATA        GPIO_PORTD_DATA_R
#define LCD_CONTROL     GPIO_PORTB_DATA_R

#define	LCD_RS(x)       ( (x) ? (LCD_CONTROL |= 0x01) : (LCD_CONTROL &= ~0x01) )
#define LCD_EN(x)       ( (x) ? (LCD_CONTROL |= 0x02) : (LCD_CONTROL &= ~0x02) )


void lcd_command  (unsigned char); // LCD ye komut göndermeye yarar - îòïğàâêà êîìàíä
void lcd_temizle(void);          // LCD ekranı temizler - ïî òóğåöêè î÷èñòêà ıêğàíà :)
void lcd_puts   (const char*);   // LCD ye string ifade yazar
void lcd_goto   (char,char);     // LCD de satır ve stün olarak istenilen yere gider
void lcd_init   (void);          // LCD başlangıç ayarları yapılır
void lcd_putch  (char);          // LCD ye tek karakter yazmak için kullanılır.




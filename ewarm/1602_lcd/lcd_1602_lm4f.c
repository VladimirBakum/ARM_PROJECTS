
#include "inc/lm4f120h5qr.h"
#include "lcd_1602_lm4f.h"
#include "systick.h"


void EN_LATCH (void)
{
  LCD_EN(1);
  delay_us(10);
  LCD_EN(0);
}


void lcd_putch(char c)
{
  LCD_RS(1); 
  
  delay_ms(2);       
  
  LCD_DATA = ((c & 0xF0) >> 4);
  EN_LATCH();
  LCD_DATA =  (c & 0x0F);
  EN_LATCH();
}

void lcd_command(unsigned char c)
{
  LCD_RS(0);
  
  delay_ms(2);      
  
  LCD_DATA = ((c & 0xF0) >> 4);
  EN_LATCH();
  LCD_DATA =  (c & 0x0F);
  EN_LATCH();
}

void lcd_cls(void)
{  
  lcd_command(0x01);
  delay_ms(10);
}

void lcd_puts(const char* s)
{ 
  while(*s)
    lcd_putch(*s++);
}

void lcd_goto(char x, char y)
{ 
  if(x==1)
    lcd_command(0x80+((y-1)%16));
  else
    lcd_command(0xC0+((y-1)%16));
}
	
void lcd_init()
{
  SYSCTL_RCGC2_R     |=  SYSCTL_RCGC2_GPIOB;	// GPIOB Aktif
  GPIO_PORTB_DIR_R   |=  0x03;          
  GPIO_PORTB_AFSEL_R &= ~0x03;          
  GPIO_PORTB_DEN_R   |=  0x03;          
  GPIO_PORTB_PCTL_R   =  0xFFFFFF00;    
  GPIO_PORTB_AMSEL_R &=  0x03;          

  SYSCTL_RCGC2_R     |=  SYSCTL_RCGC2_GPIOD;	// GPIOD Aktif
  GPIO_PORTD_DIR_R   |=  0x0F;          
  GPIO_PORTD_AFSEL_R &= ~0x0F;          
  GPIO_PORTD_DEN_R   |=  0x0F;          
  GPIO_PORTD_PCTL_R  &=  0xFFFF0000;    
  GPIO_PORTD_AMSEL_R &= ~0x0F;          

  LCD_RS(0);
  LCD_EN(0);
  delay_ms(50);

  LCD_DATA =0x03;
  EN_LATCH();
  delay_ms(6);
  LCD_DATA =0x03;
  EN_LATCH();
  delay_ms(1);
  LCD_DATA =0x03;
  EN_LATCH();
  delay_ms(1);
  LCD_DATA =0x02; // Set 4-bit interface
  EN_LATCH();
  delay_ms(1);
  
  lcd_command(0x28);  // 4 Bit , Çift 2-line mode, 5x7 dots
  lcd_command(0x08);  // Display OFF, Cursor OFF, Blink Cursor OFF
  lcd_command(0x06);  // Send Entry Mode, Addr Inc, Display not shifted

//  lcd_command(0x80);  // LCD Birinci Satýr Konumunda 
//  lcd_command(0x28);  // 4 Bit , Çift Satýr LCD
//  lcd_cls();    // Ekran Temizleniyor î÷èñòêà ýêðàíà
}

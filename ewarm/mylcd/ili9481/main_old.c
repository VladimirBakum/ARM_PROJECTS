#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h>      // for sprintf
#include <pgmspace.h>   // for memcpy_P

#define delay_us(V) __delay_cycles(V*14L)

#define KEY1  PINE_Bit4
#define KEY2  PINE_Bit5
#define KEY3  PINE_Bit6
#define KEY4  PINE_Bit7

void delay_ms (int c)
{
  do
  {
    delay_us(1000); // 1 ms
  }
  while (--c);
}

#include "font\font5x7.h"
#include "font\arial16.h"

#include "pcf8814\pcf8814.h"
#include "pcf8814\pcf8814.c"


#include "ili9481\ili9481.h"
#include "ili9481\ili9481.c"


#include "ili9481\tjpgd.h"
#include "ili9481\integer.h"
#include "ili9481\tjpgd.c"
#include "ili9481\ua.h" // picture
#include "ili9481\jpeg.c"



void main( void )
{
  pcf8814_init();
  pcf8814_clear_console(0x20); // Чистим пробелами
  
  ili9481_init();

  ili9481_rainbow16();

  ili9481_putpixel16(0,0,WHITE_COLOR);
  ili9481_putpixel16(271,0,WHITE_COLOR);
  ili9481_putpixel16(0,479,WHITE_COLOR);
  ili9481_setrect(100,200,170,290,1,RED_COLOR);
  ili9481_draw(5,400,180,100,CYAN_COLOR);
  ili9481_circle(120,300,50,GREEN_COLOR);

  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg1=0x3F;
  unsigned char reg2=0x0B;


//----------- JPEG DECODING ----------------------  1
  JDEC jd;		// TJDEC decompression object
  IODEV iodev;	// Identifier of the decompression session (depends on application)
  JRESULT rc;		// Result
  // jpeg output    
  iodev.jpic = UA;
  iodev.jsize = sizeof(UA);
  iodev.joffset = 0;

  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);


  
  while (1)
  {

    if (!KEY1)
      {
        reg1--;
        vcom_set(reg1,reg2);

        sprintf(LN7,"Reg1:%02X",reg1);
        pcf8814_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg1++;
        vcom_set(reg1,reg2);

        sprintf(LN7,"Reg1:%02X",reg1);
        pcf8814_update_console();

        delay_ms(100);
      }

    if (!KEY3)
      {
        reg2--;
        vcom_set(reg1,reg2);

        sprintf(LN8,"Reg2:%02X",reg2);
        pcf8814_update_console();

        delay_ms(100);
      }

    if (!KEY4)
      {
        reg2++;
        vcom_set(reg1,reg2);

        sprintf(LN8,"Reg2:%02X",reg2);
        pcf8814_update_console();

        delay_ms(100);
      }

  }
}

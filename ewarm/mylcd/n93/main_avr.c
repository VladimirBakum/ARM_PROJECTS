#include <iom2561.h>
#include <intrinsics.h> // for __delay procedures
#include <stdio.h> // for sprintf
#include <pgmspace.h> // for memcpy_P

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
#include "Nokia_N93\fonts.h"

#include "pcf8814\pcf8814.h"
#include "pcf8814\pcf8814.c"

#include "Nokia_N93\N93.h"
#include "Nokia_N93\N93.c"

#include "Nokia_N93\tjpgd.h"
#include "Nokia_N93\integer.h"
#include "Nokia_N93\tjpgd.c"
#include "Nokia_N93\colors24.h"
#include "Nokia_N93\jpeg.c"

void print_command (unsigned char command)
{
  unsigned char id1,id2,id3,id4,id5;
  N93_write_command(command);
  id1=N93_read_data();
  id2=N93_read_data();
  id3=N93_read_data();
  id4=N93_read_data();
  id5=N93_read_data();
  sprintf(LN1,"ID1=%02X",id1);  // FF
  sprintf(LN2,"ID2=%02X",id2);  // 83
  sprintf(LN3,"ID3=%02X",id3);  // 8A
  sprintf(LN4,"ID4=%02X",id4);  // 27
  sprintf(LN5,"ID5=%02X",id5);  // 00
  //  pcf8814_putchar16(0x30);
  pcf8814_update_console();
}

void main( void )
{

  
  pcf8814_init(); // в инит встроена очистка дисплея и установка вывода в начале экрана

  N93_init();

//  N93_rainbow16();

//  N93_putpixel(10,10,BLACK);
//  N93_draw(200,10,20,300,GREEN_COLOR);
//  N93_setrect(10,50,100,150,1,CYAN_COLOR);
//  N93_circle(100,150,85,RED_COLOR);

//  N93_putstr("Nokia N93 Display",10,10,2,BLUE_COLOR,YELLOW_COLOR);
//  N93_putstr("LCD",58,26,2,PINK_COLOR,GREEN_COLOR);
//  N93_putstr("Colour",10,26,1,RED_COLOR,ORANGE_COLOR);  
//  N93_putstr("Temperature",5,46,2,WHITE_COLOR,BLACK_COLOR);  
//  N93_putstr("MC2PA8201 CONTROLLER",2,114,0,WHITE_COLOR,BLACK_COLOR);  
//  N93_draw(120,10,5,105,BLUE_COLOR);
  

//  delay_ms(5000);
  
  pcf8814_clear_console(0x20); // Чистим пробелами

  print_command(N93_READ_DISPLAY_ID);
  
  PORTE=0xF0; // Pullup keys=1
  DDRE =0x0F; // All keys for input

  unsigned char reg=0x00;

  
//----------- JPEG DECODING ----------------------  1
  JDEC jd;		// TJDEC decompression object
  IODEV iodev;	// Identifier of the decompression session (depends on application)
  JRESULT rc;		// Result
  // jpeg output    
  iodev.jpic = COLORS24;
  iodev.jsize = sizeof(COLORS24);
  iodev.joffset = 0;

  rc = jd_prepare(&jd, input_func, jdwork, sizeof(jdwork), &iodev);
  rc = jd_decomp(&jd, output_func, SCALE);
  
  
  while (1)
  {


    if (!KEY1)
      {
        reg--;

        print_command(reg);  
        sprintf(LN8,"Reg:%02X",reg);
        pcf8814_update_console();

        delay_ms(100);
      }

    if (!KEY2)
      {
        reg++;


        print_command(reg);  
        sprintf(LN8,"Reg:%02X",reg);
        pcf8814_update_console();

        delay_ms(100);
      }

  }
}

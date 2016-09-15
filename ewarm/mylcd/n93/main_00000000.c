//---------------------------------------------------------------
// Демонстрационный проект для Stellaris Launchpad LM4F120
// для IAR ARM 6.4
// Проект ссылается на установленную библиотеку StellarisWare
// By (R)soft 5 Jan 2014 version 1.0
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_12MHZ -> 88 MHz Clock -> UART0 Work OK
// SYSCTL_SYSDIV_2, SYSCTL_USE_PLL, SYSCTL_XTAL_10MHZ -> 109 MHz Clock -> UART0 Work OK

#define UART_BUFFERED

#include <string.h> // for memcpy

//#include "inc/lm4f120h5qr.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h" // for ROM_ procedures
#include "driverlib/sysctl.h"
//#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "system.c" // Различные вспомогательные процедуры

//#include "pcf8814/pcf8814h.h"
//#include "pcf8814/pcf8814h.c"

#include "n93/n93.h"
#include "n93/n93.c"

/*
#include "n93\tjpgd.h"
#include "n93\integer.h"
#include "n93\tjpgd.c"
#include "n93\colors24.h"
#include "n93\jpeg.c"
*/



int main(void)
{

  system_init();
  
//  pcf8814_init();
  N93_init();
//  N93_rainbow16();


//  usprintf(LN1,"Texas Cortex-M4F");
//  usprintf(LN2,"Board @ %u MHz", ROM_SysCtlClockGet() / 1000000);
//  usprintf(LN3,"Real Overclock");
//  usprintf(LN4,"is 110 MHz! :o)");
//  usprintf(LN5,"Flash size %ukB", ROM_SysCtlFlashSizeGet() / 1024);
//  usprintf(LN6,"SRAM size %ukB", ROM_SysCtlSRAMSizeGet() / 1024);
//  usprintf(LN7,"Nokia 1200 LCD");
//  usprintf(LN8,"By (R)soft 2014");

//  print_command(N93_READ_DISPLAY_ID);
//  pcf8814_update_console();


//  N93_rainbow_zx();
/*
  N93_putpixel16(10,10,WHITE_COLOR);
  N93_draw(200,10,20,300,GREEN_COLOR);
  N93_draw(210,10,30,300,BLUE_COLOR);
  N93_setrect(100,50,200,150,1,GREEN_COLOR);
  N93_setrect(200,250,100,200,0,WHITE_COLOR);
  N93_circle(100,150,85,RED_COLOR);

  N93_putstr("LCD",158,46,BIG_FONT,PINK_COLOR,CYAN_COLOR);
  N93_putstr("Colour",110,46,MEDIUM_FONT,RED_COLOR,ORANGE_COLOR);
  N93_putstr("Temperature",55,86,BIG_FONT,BLUE_COLOR,TRANSPARENT);
  N93_putstr("MC2PA8201 CONTROLLER",2,124,SMALL_FONT,WHITE_COLOR,TRANSPARENT);
*/

//  DrawScreen2_zx(BARBARIAN);

//  DrawSpriteBW(10,10,0,32,128,JJACK);

/*
unsigned char n=00;
unsigned int x,y;
  for (y=0; y<200; y+=40)
  {
    for (x=0; x<300; x+=50)
    {
      if (n>34) n=34;
      DrawSpriteNodes(x,y,n++,ARC);
    }
  }
*/
  
  
//  pcf8814_update_console();

//  N93_putstr("Nodes of Yesod",2,225,BIG_FONT,WHITE_COLOR,BLACK_COLOR);
//  N93_putstr("NODES3 Sprites",120,225,BIG_FONT,RED_COLOR,BLACK_COLOR);
  /*
  delay_ms(5000);
  DrawScreen_zx(DIZZY2);
  delay_ms(5000);
  DrawScreen_zx(TURBO);
  delay_ms(5000);
  DrawScreen_zx(TOPGUN);
  delay_ms(5000);
  DrawScreen_zx(PEDRO);
  delay_ms(5000);
  DrawScreen_zx(DIVER);
  delay_ms(5000);
  DrawScreen_zx(DEATHCHASE);
  delay_ms(5000);  
  DrawScreen_zx(BARBARIAN);
  delay_ms(5000);
*/
  
/*
  unsigned int i;
  for (i=0;i<320;i=i+10)
  {
    N93_draw(0,320,240,i,RED_COLOR);
  }
  for (i=0;i<320;i=i+10)
  {
    N93_draw(240,0,0,i,YELLOW_COLOR);
  }
*/
  
  
/*  
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
*/  
  
      
  while (1) 
  {

    unsigned char chudik=8;
    unsigned char goblin=16;
    unsigned char worm=28;
    unsigned char fish=34;
    unsigned char olli=0;
    unsigned char nodes=0;
    unsigned char kiss=30;
    unsigned char hit=9;
    unsigned char ghost=47;
    unsigned char cook=0;
    unsigned char stars=27;
    unsigned char roll=10;
    unsigned char alien=0;
    unsigned int x;
    for (x=0;x<257;x+=2)
    {
      DrawSpriteNodes (x,0,chudik,NODES2);
      DrawSpriteNodes (x,25,goblin,NODES2);
      DrawSpriteNodes (x,60,worm,NODES2);
      DrawSpriteNodes (x,80,fish,NODES2);
      DrawSpriteNodes (x,105,olli,OLLI1);
      DrawSpriteNodes (x,135,nodes,NODES1);
      DrawSpriteNodes (x+32,135,alien,ARC);
      //      DrawSpriteNodes (0,180,kiss,OLLI1);
//      DrawSpriteNodes (50,180,hit,OLLI2);
//      DrawSpriteNodes (140,180,ghost,OLLI2);
//      DrawSpriteNodes (180,180,cook,OLLI2);
//      DrawSpriteNodes (200,180,65,OLLI2);
//      DrawSpriteNodes (260,180,stars,OLLI1);
      DrawSpriteNodes (x,200-TABLE_Y[roll-10],roll,NODES1);

      delay_ms(80);

      ClearSprite (x,0,2,32,BLACK_COLOR);
      ClearSprite (x,25,2,32,BLACK_COLOR);
      ClearSprite (x,60,2,32,BLACK_COLOR);
      ClearSprite (x,80,2,32,BLACK_COLOR);
      ClearSprite (x,105,2,32,BLACK_COLOR);
      ClearSprite (x,135,2,32,BLACK_COLOR);
      ClearSprite (x+32,135,2,32,BLACK_COLOR);
      ClearSprite (x,200-TABLE_Y[roll-10],24,32,BLACK_COLOR);      

      //========== DEBUG ===================
//      usprintf(LN2,">%02x", TABLE_Y[roll-10]);
//      N93_putstr(LN2, 0, 0, BIG_FONT,RED_COLOR,BLACK_COLOR);
      //====================================
      
//      OLLI_putstr("abcd efgh ijk l mnop (((",0,176);
//    SCORE (abcd) STAGE(efgh) HIGH(ijk) ?(l) ENERGY (mnop)
      
      chudik++;
      goblin++;
      worm++;
      fish++;
      olli++;
      nodes++;
      kiss++;
      hit++;
      ghost++;
      cook++;
      stars++;
      roll++;
      alien++;
      
      if (chudik > 11) chudik=8;
      if (goblin > 23) goblin=16;
      if (worm > 31) worm=28;
      if (fish > 37) fish=34;
      if (olli > 3) olli=0;
      if (nodes > 7) nodes=0;
      if (kiss > 37) kiss=30;
      if (hit > 14) hit=9;
      if (ghost > 56) ghost=47;
      if (cook > 1) cook=0;
      if (stars > 29) stars=27;
      if (roll > 27) roll=10;
      if (alien > 7) alien=0;
      
    }

    ClearSprite (x,0,64,232,BLACK_COLOR);

    
    /*
    unsigned char n=0;
    unsigned char e=0;
    unsigned char f=12;
    unsigned int x;
    for (x=0;x<277;x=x+2)
    {
      DrawSpriteBW (x,200,n,3,33,NODES);
      DrawSpriteBW (x,100,e,3,32,EXOLON1);
      DrawSpriteBW (x,150,f,3,32,EXOLON1);
      n++;
      e++;
      f++;

      if (n>7) n=0;
      if (e>9) e=0; // Счет от 0 до 9
      if (f>21) f=12;  
      delay_ms(150);
      ClearSprite (x,200,2,33,BLACK_COLOR);
      ClearSprite (x,100,2,32,BLACK_COLOR);
      ClearSprite (x,150,2,32,BLACK_COLOR);
    }

    DrawSpriteBW (x,100,10,3,32,EXOLON1); // эксолон1 присел
    DrawSpriteBW (x,150,22,3,32,EXOLON1); // эксолон2 присел    

    for (n=10;n<17;n++)
     {
        DrawSpriteBW (x,200,n,3,33,NODES);
        delay_ms(150);
     }
    
*/    
    
/*
    unsigned int i;
    for (i=0;i<210;i++)
    {
        N93_putstr("LCD",i,276,2,WHITE_COLOR,BLACK_COLOR);
        delay_ms(20);
    }
*/

  

  
  }

}

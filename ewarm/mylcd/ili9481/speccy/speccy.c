//=============================================
//====== Z X   S P E C C Y   P R O C S ========
//=============================================
// Special for TFT8K2456 and ILI9481
//
// Version 1.0 By (R)soft 03-05-2014
//


#include "speccy.h"


#include "scr.h" // ZX screens
//#include "jetpac.h" // NES nonstandart sprites
//#include "jjack.h"
//#include "nodesprt.h" // ZX Sprites
#include "nodes.h" // ZX Sprites from original game "Nodes of Yesod"
#include "arc.h" // ZX Sprites from Arc of Yesod
//#include "exolon.h" // ZX Sprites
#include "lisa.h"




//--------------------------------------
// Draw 8x8 pixels of standart ZX screen

void Draw8x8 (unsigned char col, unsigned char row, const unsigned char *pScreen)
{
  unsigned int p;
  unsigned char ink, paper;
  unsigned char line, byte_img, byte_attr;  

  // ������ ������� ���������� 8�8 ��������
  ili9481_Window(col*8, row*8, col*8+7, row*8+7);
  
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  p=ZX_ADDR[row]+col; // ����� ����������

  byte_attr = pScreen [6144 + (32*row) + col]; // ������� ���������� ����� ���� ���
  ink = (byte_attr&0x07); // ���� ������
  paper = ((byte_attr>>3)&0x07); // ���� ����
  if (byte_attr&0x40)     // ����������� ������� ��������
   {
     ink = ink + 8;
     paper = paper + 8;
   }

  for (line=0; line<8; line++)
  {
        byte_img = pScreen [p]; // ���� ��������

        if (byte_img & 0x80) 
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x40)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x20)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x10)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x08)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x04)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x02)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x01)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
      p=p+256; // Next line addr (increment high byte)    
  }
}

//--------------------------------------------------------
// ����� ����������� �������� .scr ZX Spectrum �����������
// input: Pointer to screen
// Use fill 8x8 (fast algorithm)

void DrawScreen2_zx(const unsigned char *pScreen)
{
  unsigned char col, row;

  for (row=0; row<24; row++) // ���� �����
  {
    for(col=0; col<32; col++) // ���� ����������
    {
      Draw8x8(col, row, pScreen);
    }
  }
}


//---------------------------------------------------------
// ����� �����-������ ������� ��������� �������
// � �������� ����������� ��� �����

void DrawSpriteBW (unsigned int x, unsigned int y, unsigned int numsprite,
                   unsigned char width, unsigned char height, const unsigned char *pSprites)
{
  unsigned int p; // ��������� �� ������
  p=width*height*numsprite;

  unsigned int ink, paper;
  ink = WHITE_COLOR;
  paper = BLACK_COLOR;
  
  unsigned int bytes; // ���������� ���� �������
  unsigned char byte_img;

  ili9481_Window(x, y, (x + (width*8) - 1), (y + height - 1));

  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (bytes=0; bytes<(width*height); bytes++) // ���� ������� ������ �� �������
    {
        byte_img = pSprites [p++]; // ���� ��������

        if (byte_img & 0x80) 
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x40)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x20)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x10)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x08)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x04)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x02)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x01)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          } 
    }

}


//---------------------------------------------------------
// ����� �����-������ ������� ��������� �������
// � ������������� ����������
// � �������� ����������� ��� �����

void DrawSpriteNonStandard (unsigned int x, unsigned int y, unsigned int numsprite,
                   unsigned char width, unsigned char height, const unsigned char *pSprites)
{
  unsigned int p; // ��������� �� ������
  p=width*height*numsprite;

  unsigned int ink, paper;
  ink = WHITE_COLOR;
  paper = BLACK_COLOR;
  
  unsigned char bytes, lines;
  unsigned char byte_img;

  ili9481_Window(x, y, (x + (width*8) - 1), (y + height - 1));
  
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (lines=0; lines < height; lines++)
  {
    for (bytes=0; bytes < width; bytes++) // ���� ������� ������ �� �������
    {
        byte_img = pSprites [p++]; // ���� ��������

        if (byte_img & 0x80) 
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x40)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x20)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x10)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x08)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x04)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x02)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
        if (byte_img & 0x01)
          {
            ili9481_wr_dat(ink>>8);
            ili9481_wr_dat(ink);
          }
          else               
          {
            ili9481_wr_dat(paper>>8);
            ili9481_wr_dat(paper);
          }
    }
    p=p+32-width; // ������������� ���������
  }

}


//-----------------------------------
// �������� ������� �� �����������,
// ��������� ������� � �����
// width � ��������
void ClearSprite (unsigned int x, unsigned int y,
                   unsigned char width, unsigned char height, unsigned int color)
{
  unsigned int pixels;
  
  ili9481_Window(x, y, (x + width - 1), (y + height - 1));
  
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (pixels=0; pixels<(width*height); pixels++) // ���� �������� ��������
    {
      ili9481_wr_dat(color>>8);
      ili9481_wr_dat(color);
    }

}

//---------------------------------------------------------
// ����� ������� ���� Nodes of Yesod
// � �������� ����������� ��� �����.
// ������ �������� �������� ��� ����� ����� ������ ��������:
// 1 - x_size in pixels
// 2 - y_sixe in pixels
// 3 - color attribute

void DrawSpriteNodes (unsigned int x, unsigned int y, unsigned int numsprite,
                      const unsigned char *pSprites)
{
  unsigned char width_bytes, width, height; // size of sprite
  unsigned char attribute; // color of sprite
  unsigned int i;
  unsigned int p=0; // ��������� �� ������
  if (numsprite>0) // ����� �������� ������� �����������
  {
    for (i=0; i<numsprite; i++)
    {
      width = pSprites[p++];
      height = pSprites[p++];

      width_bytes = width>>3;
      if (width%8) width_bytes++;

      p=p + (width_bytes*height) + 1; // � +1 (���� �������� ���������)
    }
  }


  width = pSprites[p++];        // 1 byte - width
  height = pSprites[p++];       // 2 byte - height
  attribute = pSprites[p++];    // 3 byte - attribute

  unsigned int ink, paper;
  ink = (attribute&0x07); // ���� ������
  paper = ((attribute>>3)&0x07); // ���� ����
  if (attribute&0x40)     // ����������� ������� ��������
   {
     ink = ink + 8;
     paper = paper + 8;
   }

//*********************
//  paper++; //!!!DEBUG
//*********************

  //������� ������ �� 8 � ����������� � ������� �������
  width_bytes = width>>3;
  if (width%8) width_bytes++;

//***********************************    
//  usprintf(LN6,"WIDTH=%02X", width_bytes);    //!!!DEBUG
//***********************************  

  unsigned int bytes; // ���������� ���� �������
  unsigned char byte_img;
  
  ili9481_Window(x, y, (x + (width_bytes*8) - 1), (y + height - 1));
  
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);

  for (bytes=0; bytes<(width_bytes*height); bytes++) // ���� ������� ������ �� �������
    {
        byte_img = pSprites [p++]; // ���� ��������

        if (byte_img & 0x80) 
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x40)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x20)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x10)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x08)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x04)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x02)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
        if (byte_img & 0x01)
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[ink]));
          }
          else               
          {
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]>>8));
            ili9481_wr_dat((unsigned char)(ZX_ATTR[paper]));
          }
    }

//****************** DEBUG ***************************  
//  usprintf(LN2,"%ux%u", width, height);
//  N93_putstr(LN2, x, y+height+1, SMALL_FONT,CYAN_COLOR,BLACK_COLOR);
//****************************************************

}

//-------------------------------------------------------------
//  OLLI_putstr(55, 86, "Temperature");
// ������ ������� ������� OLLI �� ����������� x, y

void OLLI_putstr(unsigned int x, unsigned int y, char *pString) 
{ 
    // loop until null-terminator is seen 
    while (*pString != '\0') 
    { 
      // draw the character 
      DrawSpriteBW (x,y,(*pString++)-0x20,1,8,FONT_OLLI);
      // advance the y position 
      x = x + 8;
      if (x > ILI9481_WIDTH) x = ILI9481_WIDTH;
      if (y > ILI9481_HEIGHT) break; 
    } 
} 


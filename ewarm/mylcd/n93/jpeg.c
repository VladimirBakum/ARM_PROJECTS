#define VERTICAL    0
#define HORIZONTAL  1

unsigned char jpeg_type=VERTICAL;

typedef struct 
{
  const unsigned char * jpic; // уазатель на массив с JPEG картинкой
  WORD jsize;                         // размер массива
  WORD joffset;                       // текущая позиция при чтении массива
} IODEV;

//===========================================================
// Функция декодирования кусочка JPEG в буфер

unsigned int input_func (JDEC * jd, unsigned char * buff, unsigned int ndata) 
{
  IODEV * dev = (IODEV *) jd->device;
  // Проверяем выход за границы массива
  ndata = dev->jsize - dev->joffset  > ndata ? ndata : dev->jsize - dev->joffset;
  // Копируем фрагмент из массива в буфер
  if(buff) 
    {
//      memcpy_P(buff, dev->jpic + dev->joffset, ndata);
      unsigned int i;
      for (i=0;i<ndata;i++)
        {
          // Копирование из флешь в буфер RAM
          ((unsigned char*)buff)[i] = ((dev->jpic + dev->joffset)[i]);
        }    
    }
  // Увеличиваем текущее смещение
  dev->joffset += ndata;
  // Возвращаем реальный размер скопированных данных
  return ndata;
}

//=================================================================
// Функция вывода декодированного bmp из буфера на LCD
// Переменаня jpeg_type определяет портретную или альбомную фото

unsigned int output_func(JDEC * jd, void * bitmap, JRECT * rect) 
{

  unsigned short * bmp = (unsigned short *) bitmap;

  unsigned int x;
  unsigned int y;
  unsigned int i = 0;
//  unsigned int color;

  // Цикл вывода кусочка изображения
  for(y = rect->top; y <= rect->bottom; y++) 
    {
      for(x = rect->left; x <= rect->right; x++) 
        {
          //LCD_SetPoint(y, x, bmp[i++]);
          if (jpeg_type==VERTICAL)
            {
/*              color=bmp[i++];
              red=(color>>3)&0x1F;  // в каждом цвете отбрасываем лишние биты путём сдвига и маски
              green=(color>>2)&0x3F;
              blue=(blue>>3)&0x1F;
              out_color=((blue<<11)|(green<<5)|red); // собираем все цвета воедино в одном слове
*/
              N93_putpixel16(x,y,bmp[i++]);
            }
          else
            {
              N93_putpixel16(y,x,bmp[i++]);
            }
        }
    }

  return 1;	// Continue to decompress
}


#define SCALE   0     // Output scaling 0: 1/1, 1: 1/2, 2: 1/4 or 3: 1/8

unsigned char jdwork[3100];





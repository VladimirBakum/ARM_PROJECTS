// JPEG decoder
// Version 1.1 modified by (R)soft 20-04-2014

//#define VERTICAL    0
//#define HORIZONTAL  1
//#define SCALE   0     // Output scaling 0: 1/1, 1: 1/2, 2: 1/4 or 3: 1/8

//unsigned char jpeg_type=VERTICAL; // В версии с ili9481_Windows не нужна

unsigned char jdwork[3100];

unsigned int x_jpeg, y_jpeg; // Координаты вывода jpg

typedef struct 
{
  const unsigned char * jpic; // уазатель на массив с JPEG картинкой
  WORD jsize;                 // размер массива
  WORD joffset;               // текущая позиция при чтении массива
} IODEV;

//===========================================================
// Функция декодирования кусочка JPEG в буфер
// For Flash Memory

unsigned int input_func (JDEC * jd, unsigned char * buff, unsigned int ndata) 
{
  IODEV * dev = (IODEV *) jd->device;

  // Проверяем выход за границы массива
  ndata = dev->jsize - dev->joffset  > ndata ? ndata : dev->jsize - dev->joffset;

  // Копируем фрагмент из массива в буфер
  if(buff) 
    {
      memcpy(buff, dev->jpic + dev->joffset, ndata);
    }

  // Увеличиваем текущее смещение
  dev->joffset += ndata;

  // Возвращаем реальный размер скопированных данных
  return ndata;
}

//=====================================================================
// Функция вывода декодированного bmp из буфера на LCD
// For Flash Memory

unsigned int output_func(JDEC * jd, void * bitmap, JRECT * rect) 
{

  unsigned short * bmp = (unsigned short *) bitmap;

  unsigned int i = 0;

  // Цикл вывода кусочка изображения
  // (Версия с Windows не намного быстрее поточечного вывода)
  ili9481_Window( (x_jpeg + rect->left), (y_jpeg + rect->top), \
                  (x_jpeg + rect->right), (y_jpeg + rect->bottom) );
  ili9481_wr_cmd(ILI9481_MEMORY_WRITE);
  
  unsigned short tmp = (rect->bottom - rect->top + 1)*(rect->right - rect->left + 1);
  unsigned short c;
  for (c=0; c<tmp; c++)
  {
    ili9481_wr_dat((unsigned char)(bmp[i]>>8));
    ili9481_wr_dat((unsigned char)(bmp[i++]));
  }

  
  /*for(y = rect->top; y <= rect->bottom; y++) 
  {
      for(x = rect->left; x <= rect->right; x++) 
        {
          if (jpeg_type==VERTICAL)
            {
              ili9481_putpixel16(x,y,bmp[i++]);
            }
          else
            {
              ili9481_putpixel16(y,x,bmp[i++]);
            }
        }
    }*/

  return 1;	// Continue to decompress
}


//------------------------------------------------------------------------- 
// Входная функция для microSD
/* JPEG file loader                  */
/* User defined call-back function to input JPEG data */

static
UINT tjd_input (
    JDEC* jd,       /* Decoder object */
    BYTE* buff,     /* Pointer to the read buffer (NULL:skip) */
    UINT ndata         /* Number of bytes to read/skip from input stream */
)
{
  UINT rb;
  FIL *file = (FIL*)jd->device;    /* Input stream of this session */
 
  if (buff) 
  { /* Read ndata bytes from the input strem */
    f_read(file, buff, ndata, &rb);
    return rb;  /* Returns number of bytes could be read */
  } 
  else 
  {    /* Skip nd bytes on the input stream */
      return (f_lseek(file, f_tell(file) + ndata) == FR_OK) ? ndata : 0;
  }
}


//-----------------------------------------------------------------------
// Вывод JPG из microSD на LCD по координатам x, y
// Usage: out_jpg(100, 100, "file.jpg");
// Return: Error Code

unsigned char out_jpg (unsigned int x, unsigned int y, const char *filename) // name file of opened JPG
{
  JDEC jd;        /* Decoder object (124 bytes) */
  unsigned char rc;
  BYTE scale;
  x_jpeg = x;
  y_jpeg = y;

  // Установка цвета и координат для текста
  ili9481_setcolor(CYAN_COLOR, TRANSPARENT);
  ili9481_setpos(0, 460);
  
  rc = f_open(&file, filename, FA_OPEN_EXISTING|FA_WRITE|FA_READ);
  
  if (rc!=FR_OK)
  {
    UARTprintf("rc=%d ERROR OPEN!!!\n", rc);
    ili9481_printf("%s Open Error=%d", filename, rc);
    return rc;
  }
  else 
  {
    UARTprintf("File %s opened!\n", filename);
  }

//  ili9481_clear(0);  /* Clear screen */

  /* Prepare to decompress the file */
  rc = jd_prepare(&jd, tjd_input, jdwork, sizeof(jdwork), &file);

  if (rc == JDR_OK) 
  {
    /* Determine scale factor */
    for (scale = 0; scale < 3; scale++) 
    {
      if ((jd.width >> scale) <= ILI9481_WIDTH && (jd.height >> scale) <= ILI9481_HEIGHT) break;
    }
    
    /* Display size information at bottom of screen */
    UARTprintf("Size:%ux%u scale: 1/%u\n", jd.width, jd.height, 1 << scale);

    /* Start to decompress the JPEG file */
    rc = jd_decomp(&jd, output_func, scale); /* Start to decompress */
    
    ili9481_printf("%s Size:%ux%u Scale: 1/%u\n", filename, jd.width, jd.height, 1 << scale);    

  }
  else 
  {
        /* Display error code */
        UARTprintf("Error: %d\n", rc);  //error return code, furthur searching. JDR_MEM1
        ili9481_printf("%s Error=%d", filename, rc);
        return rc;
   }

  f_close(&file);
  UARTprintf("File closed.\n");
  return rc; // Return Error code after decompress
}



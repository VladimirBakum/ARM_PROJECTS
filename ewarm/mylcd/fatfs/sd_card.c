//*****************************************************************************
//
// sd_card.c - Example program for reading files from an SD card.
//
// Copyright (c) 2011-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F232 Firmware Package.
//
//*****************************************************************************

#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "ili9481/bmp/bmp.h"
#include "ili9481/jpeg/tjpgd.h"
#include "tsc2046/tsc2046.h"
#include "ili9481/speccy/speccy.h"
#include "ili9481/png/lodepng.h"

#define PATH "/mods" // Пути неисповедимы


//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
//Fat stuff

FATFS fso;  // g_sFatFs      The FATFS structure (file system object) holds dynamic work area of individual logical drives
DIR dir;    // g_sDirObject   The DIR structure is used for the work area to read a directory by f_oepndir, f_readdir function
FILINFO fileInfo; // g_sFileInfo The FILINFO structure holds a file information returned by f_stat and f_readdir function
FIL file;   // g_sFileObject The FIL structure (file object) holds state of an open file



//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a timer tick
// every 10 ms for internal timing purposes.
// Сюда вставляем другие процедуры в прерываниях, типа опроса кнопок.
//
//*****************************************************************************
void
SysTickHandler(void)
{
  //
  // Call the FatFs tick timer.
  //
  disk_timerproc();

    
}

//------------------------------------------------------------
// File list to LCD

int file_list( void)
{
  unsigned long TotalSize = 0;
  unsigned long FileCount = 0;
  unsigned long DirCount = 0;
  FRESULT rc;
  FATFS *pFatFs;

  rc = f_opendir(&dir, ".");

  if (rc) return(rc);

  ili9481_setpos(0, 2); // Set Output position

  while(1)
    {
        rc = f_readdir(&dir, &fileInfo);

        if (rc) return(rc);

        // Если имя файла пусто, то это конец списка файлов
        if(!fileInfo.fname[0])  break;

        // Если это директория, то увеличиваем счетчик директорий
        if(fileInfo.fattrib & AM_DIR) DirCount++;
        else   // Иначе, увеличиваем счетчик файлов и считаем сумму длинны файлов
        {
            FileCount++;
            TotalSize += fileInfo.fsize;
        }

        // Вывод информации о текущей записи
        ili9481_printf("%c%c%c%c%c %9u %s\n",
                    (fileInfo.fattrib & AM_DIR) ? 'D' : '-',
                    (fileInfo.fattrib & AM_RDO) ? 'R' : '-',
                    (fileInfo.fattrib & AM_HID) ? 'H' : '-',
                    (fileInfo.fattrib & AM_SYS) ? 'S' : '-',
                    (fileInfo.fattrib & AM_ARC) ? 'A' : '-',
                     fileInfo.fsize,
                     fileInfo.fname);
    }

    // Цикл окончен, вывод информации о кол-ве файлов и общей длине
    ili9481_printf("\n%4u File(s),%10u bytes total\n%4u Dir(s)",
                       FileCount, TotalSize, DirCount);

    // Сколько места осталось
    rc = f_getfree("/", &TotalSize, &pFatFs);

    if (rc) return(rc);

    ili9481_printf(", %10uK bytes free\n", TotalSize * pFatFs->csize / 2);
    
    f_closedir(&dir); // Close opened directory
    
    return(0);
}



//----------------------------------------------------
// Вывод картинки ZX Spectrum на LCD из microSD
// filename - имя файла с расширением scr
// Usage: out_scr("barbarian.scr");
// Return: Error Code

unsigned char out_scr (const char *filename)
{
  // Установка цвета и координат для вывода текста
  
  ili9481_setcolor(CYAN_COLOR, TRANSPARENT);
  ili9481_setpos(0,460);

  unsigned char rc; // Ошибка
  
  // Флаги чтения, записи, открытие уже имеющегося файла
  rc=f_open(&file, filename, FA_OPEN_EXISTING|FA_WRITE|FA_READ);
  
  if (rc!=FR_OK || file.fsize!=6912 )
  {
    return rc; // Error open file
  }

  f_read(&file, &sdbufer, 6912, &br); // Читаем весь файл в буфер

  DrawScreen2_zx(sdbufer);
  
  f_close(&file);

  
  return 0;
}


//-------------------------------------------------------------------------
// Выводит на экран файлы директории path "." или "/files"

unsigned char show_files (char* path)
{
    unsigned char rc;
    FILINFO fno;
    DIR dir;
    char *p;
    char pos;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    rc = f_opendir(&dir, path);  /* Open the directory */
    if (rc == FR_OK) 
    {

      for (;;) 
        {
            rc = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (rc != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) 
            {    /* It is a directory */
              fColor = RED_COLOR;
//              ili9481_printf("/%s\n", fn); // Вывод имени директории
            } else 
            {    /* It is a file. */
              // Поиск расширения файла
              p = strchr(fno.fname, '.'); // Указатель на адрес первой точки в имени файла
              pos = p-fno.fname+1; // Номер позиции точки в имени файла

              fColor = WHITE_COLOR; // По умолчанию - файлы белым цветом

              if (fno.fname[pos]=='M' & fno.fname[pos+1]=='O' & fno.fname[pos+2]=='D')
                fColor = YELLOW_COLOR; // Выделяем цветом требуемый файл

              if (fno.fname[pos]=='P' & fno.fname[pos+1]=='N' & fno.fname[pos+2]=='G')
              {
                fColor = ORANGE_COLOR; // Выделяем цветом требуемый файл
#ifdef PNGENABLE
                ili9481_clear(BLACK_COLOR); // Очистка экрана
                rc = out_png(10, 10, fn); // Координаты x, y
                
                if (rc == 0) // Если не было ошибки то ждем нажатия
                {
                  delay_ms(200);
                  #ifdef TOUCH
                  wait_touch();
                  #else
                  wait_key();
                  #endif //TOUCH
                }
#endif //PNGENABLE
              }

              if (fno.fname[pos]=='B' & fno.fname[pos+1]=='M' & fno.fname[pos+2]=='P')
              {
                fColor = CYAN_COLOR; // Выделяем цветом требуемый файл
#ifdef BMPENABLE                
                ili9481_clear(BLACK_COLOR); // Очистка экрана
                rc = out_bmp(0, 0, fn); // Координаты x, y
                
                if (rc == 0) // Если не было ошибки то ждем нажатия
                {
                  #ifdef TOUCH
                  wait_touch();
                  #else
                  wait_key();
                  #endif //TOUCH
                }
#endif //BMPENABLE
              }

              if (fno.fname[pos]=='J' & fno.fname[pos+1]=='P' & fno.fname[pos+2]=='G')
              {
                fColor = YELLOW_COLOR; // Выделяем цветом требуемый файл
#ifdef JPEGENABLE
                ili9481_clear(BLACK_COLOR);                
                rc = out_jpg(0, 0, fn); // Вывод JPG по координатам x, y
                
                if (rc == 0)
                {
                  #ifdef TOUCH
                  wait_touch();
                  #else
                  wait_key();
                  #endif //TOUCH
                }
#endif //JPEGENABLE
              }

              // Расширение SCR - ZX Spectrum standard screen
              if (fno.fname[pos]=='S' & fno.fname[pos+1]=='C' & fno.fname[pos+2]=='R')
              {
#ifdef SPECCY
                ili9481_clear(BLACK_COLOR);                
                rc = out_scr(fn);
                
                if (rc == 0)
                {
                  #ifdef TOUCH
                  wait_touch();
                  #else
                  wait_key();
                  #endif //TOUCH
                }
#endif //SPECCY                
              }

              
//              ili9481_printf("%s/%s\n", path, fn); // Вывод имени файла
            }
        }
        f_closedir(&dir);
    }

    return rc;
}



//================================================

typedef struct{
  char szFileExt[5];
  void (*player)(void);
}FileHandler;

//void out_jpg (const char *filename);
//int out_bmp (const char *filename);

//Define file format handlers
/*FileHandler g_fhHandlers[]={{".JPG", out_jpg},
                            {".BMP", out_bmp}};
*/


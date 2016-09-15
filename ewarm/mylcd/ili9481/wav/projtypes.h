#ifndef _TYPES_
#define _TYPES_

#include "settings.h"

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define s8 signed char
#define s16 signed short
#define s32 signed int

#define true 1
#define false 0
#define bool u8

#define LEFT 0
#define RIGHT 1

typedef __packed struct _TWavBlock
{
    u16 size;
    u8 data[512];
}TWavBlock;

typedef __packed struct _TRIFFsection
{
  char id[4];         // 4 byte
  u32 len;             // 4 byte
}TRIFFsection;

typedef __packed struct _TWavFmtGeneric
{
  u16 compression;     // 2 byte
  u16 channels;        // 2 byte
  u32 samplerate;     // 4 byte
  u32 bytespersecond; // 4 byte
  u16 blockalign;     // 2 byte
  u16 bitspersample;  // 2 byte
}TWavFmtGeneric;

#endif //_TYPES_

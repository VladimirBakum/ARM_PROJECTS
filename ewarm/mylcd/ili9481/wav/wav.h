#ifndef _WAV_
#define _VAV_

#include "projtypes.h"

#define WAV_BUF_BLOCKS         16
#define WAV_BUF_BLOCKS_MASK     (WAV_BUF_BLOCKS-1)
#define WAVQUEUE_SIZE          16
#define WAVQUEUE_SIZE_MASK     (WAVQUEUE_SIZE-1)
#define WAVFILENAME_MAX_LEN     16

//коды ошибок, возвращаемые OpenWaveFile
#define WAVFILE_NOT_FOUND        (1 << 0)
#define WAVFILE_HEADER_NO_DATA  (1 << 1)
#define WAVFILE_SECTION_NO_DATA (1 << 2)
#define WAVFILE_FORMAT_ERR      (1 << 3)
#define WAVFILE_EFSL_ERR         (1 << 6)

extern TWavBlock wavbuf [WAV_BUF_BLOCKS];
extern u8 wavIn, wavOut;
extern u32 bytes_to_read;
extern char wavqueue [WAVQUEUE_SIZE][WAVFILENAME_MAX_LEN];
extern u8 inWav, outWav;

void wavreadPoll (void);
u8 OpenWaveFile (char* name, u32* datalen);
void ConfigureTc1 (void);

#endif // _WAV_

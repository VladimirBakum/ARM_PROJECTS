#include <string.h>
#include <board.h>
#include <tc/tc.h>
#include <aic/aic.h>
#include "wav.h"
#include "efs.h"
#include "vars.h"
#include "isr.h"
#include "pins.h"

TWavBlock wavbuf [WAV_BUF_BLOCKS];
u8 wavIn, wavOut;
static EmbeddedFile wfile;
u32 bytes_to_read;
char wavqueue [WAVQUEUE_SIZE][WAVFILENAME_MAX_LEN];
u8 inWav, outWav;
TWavFmtGeneric WAVEfmt;

u8 StoredInBuffer (u8 idxIN, u8 idxOUT, u8 bufsize)
{
    if (idxIN >= idxOUT)
        return (idxIN - idxOUT);
    else
        return ((bufsize - idxOUT) + idxIN);
}

////////////////////////////////////////////////////////////////////////////////
// ������ TC1 ������������ ��� ������ ������� ���������������� ����� � �������
//  PWM (�������� �� 16000 ��).
void ConfigureTc1 (void)
{
    u32 tcclks; //���� TCCLKS � �������� TC_CMR (����� ������������)
    u32 div;    //����������� ������� ������� ������� ��� ����� ������� tcclks

    /// ������������� Timer Counter 1 �� ������� ��������� ������� ����� TC1_FREQ.
    // Enable peripheral clock TC1
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_TC1;

    #define TC1_FREQ 16000
    // Configure TC1 for a TC1_FREQ Hz frequency and trigger on RC compare
//    if (TC_FindMckDivisor(TC1_FREQ, BOARD_MCK, &div, &tcclks))
//    {
//        TC_Configure(AT91C_BASE_TC1, tcclks | AT91C_TC_CPCTRG);
//        AT91C_BASE_TC1->TC_RC = (BOARD_MCK / div) / TC1_FREQ; // timerFreq / desiredFreq
//        
//        // Configure and enable interrupt on RC compare
//        AIC_ConfigureIT(AT91C_ID_TC1, AT91C_AIC_PRIOR_HIGHEST, /*(void(*)(void))*/ISR_Tc1);
//        AT91C_BASE_TC1->TC_IER = AT91C_TC_CPCS;
//        AIC_EnableIT(AT91C_ID_TC1);
//        
//        TC_Start(AT91C_BASE_TC1);
//        //printf("TC1: TCCLKS=%u, div=%u, freq=%iHz\n\r", tcclks, div, TC1_FREQ);
//    }
//    else
//    {
//        printf("Cannot configure TC1 for freq %i Hz...\n\r", TC1_FREQ);
//    }
    div = 8;
    tcclks = 1;
    TC_Configure(AT91C_BASE_TC1, tcclks | AT91C_TC_CPCTRG);
    AT91C_BASE_TC1->TC_RC = (BOARD_MCK / div) / TC1_FREQ; // timerFreq / desiredFreq
    AIC_ConfigureIT(AT91C_ID_TC1, AT91C_AIC_PRIOR_HIGHEST, /*(void(*)(void))*/ISR_Tc1);
    AT91C_BASE_TC1->TC_IER = AT91C_TC_CPCS;
    AIC_EnableIT(AT91C_ID_TC1);
    TC_Start(AT91C_BASE_TC1);
}

////////////////////////////////////////////////////////////////////////////////
// ������� ��������� WAV-���� �� ����� name, ��������� ��� ��������� 
// ���������������. �� ������ ������� ���������� ����� ����������� *datalen,
// � ����� �������� ����, ������� � ������ � ������� ������ �����������.
// ���� ��� � �������, �� ��������� ������� 0, ����� ��������� �������� 
// ��� ������.
u8 OpenWaveFile (char* name, u32* datalen)
{
    TRIFFsection riffsection;
    u32 rifflen;
    u32 readed, portion;
    char tmpbuf[512];

    *datalen = 0;
    file_opened = false;
    //�������������� �������� �������
    if (!efs_initialized)
    {
        if ( efs_init (&efsl, 0) !=0 )
        {
            printf("����� MMC �� �����������");printf(CRLF);
            return WAVFILE_EFSL_ERR;
        }
        else
        {
            efs_initialized = true;
        }
    }
    //��������� ����
    if (0 != file_fopen(&wfile, &efsl.myFs, name, 'r'))
    {
        printf ("�� ������ ���� %s", name);printf(CRLF);
        CloseFileSystem(&wfile);
        return WAVFILE_NOT_FOUND;
    }
    file_opened = true;
    //��������� ��������� RIFF
    memset (&riffsection, 0, sizeof(TRIFFsection));
    readed = file_read(&wfile, sizeof(TRIFFsection), (u8*)&riffsection);
    if ((sizeof(TRIFFsection) != readed) || efsl.myIOman.error)
    {
        printf ("ERR read RIFF header.");printf(CRLF);
        CloseFileSystem(&wfile);
        return WAVFILE_HEADER_NO_DATA;
    }
    //�������� ��������� RIFF
    rifflen = riffsection.len;
    if (0 != memcmp(riffsection.id, "RIFF", 4) || 0 == rifflen)
    {
        printf ("ERR in RIFF header.");printf(CRLF);
        CloseFileSystem(&wfile);
        return WAVFILE_FORMAT_ERR;
    }
    //��������� ������������� ���� WAV-�����
    readed = file_read(&wfile, 4, (u8*)tmpbuf);
    if ((4 != readed) || efsl.myIOman.error)
    {
        printf ("ERR read WAVE ID.");printf(CRLF);
        CloseFileSystem(&wfile);
        return WAVFILE_HEADER_NO_DATA;
    }
    rifflen -= readed;
    //�������� ������������� ���� WAV-�����
    if (0 != memcmp(tmpbuf, "WAVE", 4))
    {
        printf ("ERR WAVE ID.");printf(CRLF);
        CloseFileSystem(&wfile);
        return WAVFILE_FORMAT_ERR;
    }
    //������ � ���������� ��� ������, ����� "fmt " � "data"
    memset (&WAVEfmt, 0, sizeof(TWavFmtGeneric));
    while (rifflen)
    {
        readed = file_read(&wfile, sizeof(TRIFFsection), (u8*)&riffsection);
        if ((sizeof(TRIFFsection) != readed) || efsl.myIOman.error)
        {
            printf ("ERR read file section.");printf(CRLF);
            CloseFileSystem(&wfile);
            return WAVFILE_SECTION_NO_DATA;
        }
        rifflen -= readed;
        //������ �������?
        if (0==memcmp("fmt ", riffsection.id, 4))
        {
            //�������� ����� ������ �������
            if (sizeof(TWavFmtGeneric) != riffsection.len)
            {
                printf ("ERR not support extra WAV format.");printf(CRLF);
                CloseFileSystem(&wfile);
                return WAVFILE_FORMAT_ERR;
            }
            //��������� ������ �������
            readed = file_read(&wfile, sizeof(TWavFmtGeneric), (u8*)&WAVEfmt);
            if ((sizeof(TWavFmtGeneric) != readed) || efsl.myIOman.error)
            {
                printf ("ERR read WAV file format section.");printf(CRLF);
                CloseFileSystem(&wfile);
                return WAVFILE_SECTION_NO_DATA;
            }
            rifflen -= readed;
            //��� ���� ��������� ������
            if (1 != WAVEfmt.compression)
            {
                printf ("ERR compression not supported");printf(CRLF);
                CloseFileSystem(&wfile);
                return WAVFILE_FORMAT_ERR;
            }
        }
        else if (0==memcmp("data", riffsection.id, 4))
        {
            if (0==WAVEfmt.channels)
            {
                printf ("ERR section 'data' before section 'fmt '");printf(CRLF);
                CloseFileSystem(&wfile);
                return WAVFILE_FORMAT_ERR;
            }
            else
            {
                //��� ��, ������� �� ������� ������ ������
                *datalen = riffsection.len;
                return 0;
            }
        }
        else
        {
            //�� ������������ ��� ������, ������ ���������� � ������
            while (riffsection.len)
            {
                if (512 >= riffsection.len)
                    portion = riffsection.len;
                else
                    portion = 512;
                readed = file_read(&wfile, portion, (u8*)&tmpbuf);
                if ((portion != readed) || efsl.myIOman.error)
                {
                    printf ("ERR dummy read section %s.", riffsection.id);printf(CRLF);
                    CloseFileSystem(&wfile);
                    return WAVFILE_SECTION_NO_DATA;
                }
                rifflen         -= readed;
                riffsection.len -= readed;
            }
        }
    }// while (rifflen): ������ ������ WAV-�����
    printf ("ERR not exist 'data' section");printf(CRLF);
    CloseFileSystem(&wfile);
    return WAVFILE_FORMAT_ERR;
}

////////////////////////////////////////////////////////////////////////////////
// ��������� ����������� ����� �� ������� wavqueue.
void wavreadPoll (void)
{
    u32 readed, portion;
    char filepath [32];

    if ((mode == MODE_IDLE)||(MODE_PLAY_DONE == mode))
    {
        if (inWav == outWav)
        {
            RADIOSEND_OFF();
            return;
        }
        mode = MODE_INIT_PLAY;
    }
    else if (MODE_INIT_PLAY == mode)
    {
        strcpy(filepath, WAVFOLDER);
        strcat(filepath, "/");
        strcat(filepath, wavqueue[outWav++]);
        outWav &= WAVQUEUE_SIZE_MASK;
        errstate = OpenWaveFile(filepath, &bytes_to_read);
        if ((0 == errstate)&&(0 != bytes_to_read))
        {
            //������ ����� ������ ������ � �����
            if (512 > bytes_to_read)
                portion = bytes_to_read;
            else
                portion = 512;
            
            readed = file_read(&wfile, portion, wavbuf[wavIn].data);
            if (readed != portion)
            {
                printf("������ ������ �����");printf(CRLF);
                CloseFileSystem(&wfile);
                bytes_to_read = 0;
                mode = MODE_IDLE;
            }
            else
            {
                RADIOSEND_ON();
                bytes_to_read -= portion;
                wavbuf[wavIn].size = portion;
                wavIn++;
                wavIn &= WAV_BUF_BLOCKS_MASK;
                if (0 == bytes_to_read)
                    CloseFileSystem(&wfile);
                mode = MODE_PLAY_FILE;
            }
        }
        else
        {
            bytes_to_read = 0;
            mode = MODE_IDLE;
        }
    }
    else if (MODE_PLAY_FILE == mode)
    {
        if (0 == bytes_to_read)
            return;
        //���� ��� ������ ��� ������
        if (WAV_BUF_BLOCKS-2 < StoredInBuffer(wavIn, wavOut, WAV_BUF_BLOCKS))
            return;
        //����� �� ����������, ������ ����� ������ ������ � �����
        if (512 > bytes_to_read)
            portion = bytes_to_read;
        else
            portion = 512;
        
        readed = file_read(&wfile, portion, wavbuf[wavIn].data);
        if (readed != portion)
        {
            printf("������ ������ �����");printf(CRLF);
            CloseFileSystem(&wfile);
            bytes_to_read = 0;
            mode = MODE_IDLE;
        }
        else
        {
            bytes_to_read -= portion;
            wavbuf[wavIn].size = portion;
            wavIn++;
            wavIn &= WAV_BUF_BLOCKS_MASK;
            if (0 == bytes_to_read)
                CloseFileSystem(&wfile);
        }
    }
}

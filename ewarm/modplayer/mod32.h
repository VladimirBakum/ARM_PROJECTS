#ifndef __MOD32_H__
#define __MOD32_H__

// Effects
#define ARPEGGIO_MOD              0x0
#define PORTAMENTOUP_MOD          0x1
#define PORTAMENTODOWN_MOD        0x2
#define TONEPORTAMENTO_MOD        0x3
#define VIBRATO_MOD               0x4
#define PORTAMENTOVOLUMESLIDE_MOD 0x5
#define VIBRATOVOLUMESLIDE_MOD    0x6
#define TREMOLO_MOD               0x7
#define SETCHANNELPANNING     0x8
#define SETSAMPLEOFFSET_MOD       0x9
#define VOLUMESLIDE_MOD           0xA
#define JUMPTOORDER_MOD           0xB
#define SETVOLUME             0xC
#define BREAKPATTERNTOROW_MOD     0xD
#define SETSPEED_MOD              0xF

// 0xE subset
#define SETFILTER             0x0
#define FINEPORTAMENTOUP      0x1
#define FINEPORTAMENTODOWN    0x2
#define GLISSANDOCONTROL      0x3
#define SETVIBRATOWAVEFORM_MOD    0x4
#define SETFINETUNE_MOD           0x5
#define PATTERNLOOP_MOD           0x6
#define SETTREMOLOWAVEFORM_MOD    0x7
#define RETRIGGERNOTE         0x9
#define FINEVOLUMESLIDEUP     0xA
#define FINEVOLUMESLIDEDOWN   0xB
#define NOTECUT               0xC
#define NOTEDELAY             0xD
#define PATTERNDELAY          0xE
#define INVERTLOOP            0xF

//Prototypes
void mod_player();
void mod_mixer();
void loadMod();
uint16_t mod_getSamplesPerTick();

#endif

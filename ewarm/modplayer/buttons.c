**************************************
                *                                      *
                *              MODULE MAP              *
                *                                      *
                ****************************************


  DEFINED ABSOLUTE ENTRIES
  PROGRAM MODULE, NAME : ?ABS_ENTRY_MOD

Absolute parts
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _..X_RSTACK_END         000021FF 
           _..X_RSTACK_BASE        00000200 
           _..X_CSTACK_END         000021FF 
           _..X_CSTACK_BASE        00000200 
           _..X_EXT_NV_SIZE        00000000 
           _..X_EXT_NV_BASE        000021FF 
           _..X_EXT_ROM_SIZE       00000000 
           _..X_EXT_ROM_BASE       000021FF 
           _..X_EXT_SRAM_SIZE      00000000 
           _..X_EXT_SRAM_BASE      000021FF 
           _..X_FLASH_BASE         000000CC 
           _..X_FLASH_CODE_END     0001FFFF 
           _..X_RSTACK_SIZE        00000030 
           _..X_CSTACK_SIZE        00000140 
           _..X_HUGE_HEAP_SIZE     00000000 
           _..X_FAR_HEAP_SIZE      00000000 
           _..X_NEAR_HEAP_SIZE     00000000 
           _..X_TINY_HEAP_SIZE     00000000 
           _..X_HEAP_SIZE          00000010 
           _..X_EEPROM_START       00000000 
           _..X_EEPROM_END         00000FFF 
           _..X_SRAM_END           000021FF 
           _..X_SRAM_TSIZE         00000000 
           _..X_SRAM_TBASE         00000000 
           _..X_SRAM_BASE          00000200 
           _..X_FLASH_END          0003FFFF 
           _..X_FLASH_NEND         0000FFFF 
           _..X_INTVEC_SIZE        000000CC 
    *************************************************************************

  FILE NAME : D:\AVR_PROJECTS\LCD5130\Release\Obj\main.r90
  PROGRAM MODULE, NAME : main

  SEGMENTS IN THE MODULE
  ======================
ABSOLUTE
  Relative segment, address: DATA 0000002E - 0000002E (0x1 bytes), align: 0
  Segment part 1. ROOT.       Intra module refs:   main
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PORTE                0000002E 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 0000002D - 0000002D (0x1 bytes), align: 0
  Segment part 2. ROOT.       Intra module refs:   main
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_DDRE                 0000002D 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 0000002C - 0000002C (0x1 bytes), align: 0
  Segment part 3. ROOT.       Intra module refs:   main
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PINE                 0000002C 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000028 - 00000028 (0x1 bytes), align: 0
  Segment part 4. ROOT.       Intra module refs:   pcf8814_cmd
                                                   pcf8814_data
                                                   pcf8814_init
                                                   pcf8814_write
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PORTC                00000028 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000027 - 00000027 (0x1 bytes), align: 0
  Segment part 5. ROOT.       Intra module refs:   pcf8814_init
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_DDRC                 00000027 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000026 - 00000026 (0x1 bytes), align: 0
  Segment part 6. ROOT.
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PINC                 00000026 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000025 - 00000025 (0x1 bytes), align: 0
  Segment part 7. ROOT.       Intra module refs:   ili9481_wr_cmd
                                                   ili9481_wr_dat
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PORTB                00000025 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000024 - 00000024 (0x1 bytes), align: 0
  Segment part 8. ROOT.       Intra module refs:   ili9481_init
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_DDRB                 00000024 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000022 - 00000022 (0x1 bytes), align: 0
  Segment part 9. ROOT.       Intra module refs:   ili9481_init
                                                   ili9481_wr_cmd
                                                   ili9481_wr_dat
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_PORTA                00000022 
    -------------------------------------------------------------------------
ABSOLUTE
  Relative segment, address: DATA 00000021 - 00000021 (0x1 bytes), align: 0
  Segment part 10. ROOT.      Intra module refs:   ili9481_init
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           _A_DDRA                 00000021 
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 0000188C - 000018A5 (0x1a bytes), align: 1
  Segment part 30.            Intra module refs:   Segment part 84
                                                   ili9481_init
                                                   pcf8814_init
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           delay_ms                0000188C 
               RSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
NEAR_F
  Relative segment, address: CODE 000000CC - 0000052B (0x460 bytes), align: 0
  Segment part 20.            Intra module refs:   pcf8814_putchar
    -------------------------------------------------------------------------
NEAR_F
  Relative segment, address: CODE 0000
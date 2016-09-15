          pcf8814_write           000018A6 
               RSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 000018BE - 000018C5 (0x8 bytes), align: 1
  Segment part 33.            Intra module refs:   pcf8814_cls
                                                   pcf8814_init
                                                   pcf8814_update_console
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_cmd             000018BE 
               calls direct
               RSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 000018C6 - 000018CD (0x8 bytes), align: 1
  Segment part 34.            Intra module refs:   Segment part 38
                                                   pcf8814_cls
                                                   pcf8814_putchar
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_data            000018C6 
               calls direct
               RSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 000018CE - 000018D9 (0xc bytes), align: 1
  Segment part 38.            Intra module refs:   pcf8814_putchar
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 000018DA - 000018FD (0x24 bytes), align: 1
  Segment part 39.            Intra module refs:   pcf8814_update_console
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_putchar         000018DA 
               calls direct
               RSTACK = 00000000 ( 00000003 )
               CSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 000018FE - 0000192B (0x2e bytes), align: 1
  Segment part 40.            Intra module refs:   pcf8814_init
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_cls             000018FE 
               calls direct
               RSTACK = 00000000 ( 00000003 )
               CSTACK = 00000000 ( 00000002 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 0000192C - 0000196F (0x44 bytes), align: 1
  Segment part 42.            Intra module refs:   main
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_init            0000192C 
               calls direct
               RSTACK = 00000000 ( 00000003 )
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 00001970 - 00001979 (0xa bytes), align: 1
  Segment part 44.            Intra module refs:   pcf8814_update_console
    -------------------------------------------------------------------------
FARCODE
  Relative segment, address: CODE 0000197A - 000019AD (0x34 bytes), align: 1
  Segment part 45.            Intra module refs:   Segment part 85
           ENTRY                   ADDRESS         REF BY
           =====                   =======         ======
           pcf8814_update_console
             
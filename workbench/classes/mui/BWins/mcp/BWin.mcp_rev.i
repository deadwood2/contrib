VERSION EQU 20
REVISION EQU 6
DATE MACRO
    dc.b '20.10.2025'
    ENDM
VERS MACRO
    dc.b 'BWin.mcp 20.6'
    ENDM
VSTRING MACRO
    dc.b '$VER: BWin.mcp 20.6 (20.10.2025) 2002 Alfonso Ranieri <alforan@tin.it>',13,10,0
    ENDM
VERSTAG MACRO
    dc.b 0,'$VER: BWin.mcp 20.6 (20.10.2025)',0
    ENDM

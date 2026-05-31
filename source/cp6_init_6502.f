C*T***********************************************************
C*T* ASMINI (6502) - runtime initialization of the packed-     *
C*T* character COMMON data for the ASM6502 Linux/gfortran port.*
C*T* Identical in purpose to the ASMZ80 ASMINI (cp6_init.f)    *
C*T* but with the 6502 opcode mnemonic table.  Linked only     *
C*T* into the asm6502 binary.                                  *
C*T***********************************************************
      SUBROUTINE ASMINI
      IMPLICIT INTEGER(A-Z)
      INCLUDE 'asm6502_c1.finc'
      CHARACTER*4 MNEM(2,96)
      DATA ((MNEM(I,J),I=1,2),J=1,96)/
     1  'ADC ','    ','AND ','    ','CMP ','    ','EOR ','    ',
     2  'LDA ','    ','ORA ','    ','SBC ','    ','STA ','    ',
     3  'ASL ','    ','LSR ','    ','ROL ','    ','ROR ','    ',
     4  'BCC ','    ','BCS ','    ','BEQ ','    ','BMI ','    ',
     5  'BNE ','    ','BPL ','    ','BVC ','    ','BVS ','    ',
     6  'BIT ','    ','CPX ','    ','CPY ','    ','DEC ','    ',
     7  'INC ','    ','JMP ','    ','JSR ','    ','LDX ','    ',
     8  'LDY ','    ','STX ','    ','STY ','    ','BRK ','    ',
     9  'CLC ','    ','CLD ','    ','CLI ','    ','CLV ','    ',
     A  'DEX ','    ','DEY ','    ','INX ','    ','INY ','    ',
     1  'NOP ','    ','PHA ','    ','PHP ','    ','PLA ','    ',
     2  'PLP ','    ','RTI ','    ','RTS ','    ','SEC ','    ',
     3  'SED ','    ','SEI ','    ','TAX ','    ','TAY ','    ',
     4  'TSX ','    ','TXA ','    ','TXS ','    ','TYA ','    ',
     5  'DATA','    ','WORD','    ','BYTE','    ','TEXT','    ',
     6  'ADDR','    ','EQU ','    ','ORG ','    ','RES ','    ',
     7  'BSS ','    ','ENT ','    ','DEF ','    ','EXT ','    ',
     8  'REF ','    ','END ','    ','NAME','    ','TITL','E   ',
     9  'PAGE','    ','EJEC','T   ','SKIP','    ','SPAC','E   ',
     A  'INCL','    ','IF  ','    ','ELSE','    ','FI  ','    ',
     1  'GOTO','    ','LIST','    ','NLIS','T   ','MAC ','    ',
     2  'MEND','    ','SET ','    ','OPTS','    ','EXTL','    ',
     3  'REFL','    ','EXTS','    ','REFS','    ','DEFS','    ',
     4  'DEFB','    ','DEFW','    ','DEFM','    ','BOUN','D   '/
      CALL S2W('    ',1,4,BLANK)
      DO 10 J=1,96
         CALL S2W(MNEM(1,J),1,4,OPTBL(1,J))
         CALL S2W(MNEM(2,J),1,4,OPTBL(2,J))
   10 CONTINUE
      DO 20 I=1,120
   20 TITLE(I)=BLANK
      TIME(1)=BLANK
      TIME(2)=BLANK
      DATE(1)=BLANK
      DATE(2)=BLANK
      RETURN
      END

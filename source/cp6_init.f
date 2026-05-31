C*T***********************************************************
C*T* ASMINI - runtime initialization of the packed-character  *
C*T* COMMON data for the ASMZ80 Linux/gfortran port.  The CP-6 *
C*T* 9-bit character packing (4 chars/36-bit word, leftmost    *
C*T* char in bits 27-35) cannot be expressed with gfortran     *
C*T* DATA statements, so the opcode mnemonic table, the blank  *
C*T* word, the title buffer and the time/date words are packed *
C*T* here at startup.  Call ASMINI before GETOPT.              *
C*T***********************************************************
      SUBROUTINE ASMINI
      IMPLICIT INTEGER(A-Z)
      INCLUDE 'asmz80_c1.finc'
      CHARACTER*4 MNEM(2,106)
      DATA ((MNEM(I,J),I=1,2),J=1,106)/
     1  'LD  ','    ','ADD ','    ','ADC ','    ','SBC ','    ',
     2  'INC ','    ','DEC ','    ','SUB ','    ','AND ','    ',
     3  'XOR ','    ','OR  ','    ','CP  ','    ','RLC ','    ',
     4  'RRC ','    ','RL  ','    ','RR  ','    ','SLA ','    ',
     5  'SRA ','    ','SRL ','    ','BIT ','    ','RES ','    ',
     6  'SET ','    ','JP  ','    ','CALL','    ','RET ','    ',
     7  'JR  ','    ','IN  ','    ','OUT ','    ','PUSH','    ',
     8  'POP ','    ','EX  ','    ','DJNZ','    ','IM  ','    ',
     9  'RST ','    ','EXX ','    ','DAA ','    ','CPL ','    ',
     A  'CCF ','    ','SCF ','    ','NOP ','    ','HALT','    ',
     1  'DI  ','    ','EI  ','    ','RLCA','    ','RLA ','    ',
     2  'RRCA','    ','RRA ','    ','LDI ','    ','LDIR','    ',
     3  'LDD ','    ','LDDR','    ','CPI ','    ','CPIR','    ',
     4  'CPD ','    ','CPDR','    ','NEG ','    ','RLD ','    ',
     5  'RRD ','    ','RETI','    ','RETN','    ','INI ','    ',
     6  'INIR','    ','IND ','    ','INDR','    ','OUTI','    ',
     7  'OTIR','    ','OUTD','    ','OTDR','    ','DATA','    ',
     8  'WORD','    ','BYTE','    ','TEXT','    ','ADDR','    ',
     9  'EQU ','    ','ORG ','    ','BSS ','    ','ENT ','    ',
     A  'DEF ','    ','EXT ','    ','REF ','    ','END ','    ',
     1  'NAME','    ','TITL','E   ','PAGE','    ','EJEC','T   ',
     2  'SKIP','    ','SPAC','E   ','INCL','    ','IF  ','    ',
     3  'ELSE','    ','FI  ','    ','GOTO','    ','LIST','    ',
     4  'NLIS','T   ','MAC ','    ','MEND','    ','SET ','    ',
     5  'OPTS','    ','EXTL','    ','REFL','    ','EXTS','    ',
     6  'REFS','    ','DEFS','    ','DEFB','    ','DEFW','    ',
     7  'DEFM','    ','BOUN','D   '/
      CALL S2W('    ',1,4,BLANK)
      DO 10 J=1,106
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

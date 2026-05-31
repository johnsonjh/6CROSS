/*T***********************************************************/
/*T*                                                         */
/*T* Copyright (c) Bull HN Information Systems Inc., 1989    */
/*T*                                                         */
/*T***********************************************************/
MSA6800: PROC MAIN NOAUTO;
%INCLUDE CP_6;
%LIST;
%INCLUDE MSA_C1;
    DCL TABLE0(0:255) CONSTANT         CHAR(4) INIT(
        '... ','NOP ','... ','... ','... ','... ','TAP ','TPA ',
        'INX ','DEX ','CLV ','SEV ','CLC ','SEC ','CLI ','SEI ',
        'SBA ','CBA ','... ','... ','... ','... ','TAB ','TBA ',
        '... ','DAA ','... ','ABA ','... ','... ','... ','... ',
        'BRA ','... ','BHI ','BLS ','BCC ','BCS ','BNE ','BEQ ',
        'BVC ','BVS ','BPL ','BMI ','BGE ','BLT ','BGT ','BLE ',
        'TSX ','INS ','PULA','PULB','DES ','TXS ','PSHA','PSHB',
        '... ','RTS ','... ','RTI ','... ','... ','WAI ','SWI ',
        'NEGA','... ','... ','COMA','LSRA','... ','RORA','ASRA',
        'ASLA','ROLA','DECA','... ','INCA','TSTA','... ','CLRA',
        'NEGB','... ','... ','COMB','LSRB','... ','RORB','ASRB',
        'ASLB','ROLB','DECB','... ','INCB','TSTB','... ','CLRB',
        'NEG ','... ','... ','COM ','LSR ','... ','ROR ','ASR ',
        'ASL ','ROL ','DEC ','... ','INC ','TST ','JMP ','CLR ',
        'NEG ','... ','... ','COM ','LSR ','... ','ROR ','ASR ',
        'ASL ','ROL ','DEC ','... ','INC ','TST ','JMP ','CLR ',
        'SUBA','CMPA','SBCA','... ','ANDA','BITA','LDAA','... ',
        'EORA','ADCA','ORAA','ADDA','CPX ','BSR ','LDS ','... ',
        'SUBA','CMPA','SBCA','... ','ANDA','BITA','LDAA','STAA',
        'EORA','ADCA','ORAA','ADDA','CPX ','... ','LDS ','STS ',
        'SUBA','CMPA','SBCA','... ','ANDA','BITA','LDAA','STAA',
        'EORA','ADCA','ORAA','ADDA','CPX ','JSR ','LDS ','STS ',
        'SUBA','CMPA','SBCA','... ','ANDA','BITA','LDAA','STAA',
        'EORA','ADCA','ORAA','ADDA','CPX ','JSR ','LDS ','STS ',
        'SUBB','CMPB','SBCB','... ','ANDB','BITB','LDAB','... ',
        'EORB','ADCB','ORAB','ADDB','... ','... ','LDX ','... ',
        'SUBB','CMPB','SBCB','... ','ANDB','BITB','LDAB','STAB',
        'EORB','ADCB','ORAB','ADDB','... ','... ','LDX ','STX ',
        'SUBB','CMPB','SBCB','... ','ANDB','BITB','LDAB','STAB',
        'EORB','ADCB','ORAB','ADDB','... ','... ','LDX ','STX ',
        'SUBB','CMPB','SBCB','... ','ANDB','BITB','LDAB','STAB',
        'EORB','ADCB','ORAB','ADDB','... ','... ','LDX ','STX ');
 
    DCL TABLE1(0:255) CONSTANT UNAL    SBIN BYTE INIT(
          0,  1,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  0,  0,  0,  0,  1,  1,  0,  1,  0,  1,  0,  0,  0,  0,
         18,  0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
          1,  1,  1,  1,  1,  1,  1,  1,  0, 65,  0, 65,  0,  0, -2, -1,
          1,  0,  0,  1,  1,  0,  1,  1,  1,  1,  1,  0,  1,  1,  0,  1,
          1,  0,  0,  1,  1,  0,  1,  1,  1,  1,  1,  0,  1,  1,  0,  1,
         90,  0,  0, 90, 90,  0, 90, 90, 90, 90, 90,  0, 90, 90, 66, 90,
        107,  0,  0,107,107,  0,107,107,107,107,107,  0,107, 75, 35, 83,
          2,  2,  2,  0,  2,  2,  2,  0,  2,  2,  2,  2,  3,122,  3,  0,
         90, 90, 90,  0, 90, 90, 90, 98, 90, 90, 90, 90, 90,  0, 90, 98,
         90, 90, 90,  0, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
         75, 75, 75,  0, 75, 75, 75, 83, 75, 75, 75, 75, 75, 43, 75, 83,
          2,  2,  2,  0,  2,  2,  2,  0,  2,  2,  2,  2,  0,  0,  3,  0,
         90, 90, 90,  0, 90, 90, 90, 98, 90, 90, 90, 90,  0,  0, 90, 98,
         90, 90, 90,  0, 90, 90, 90, 90, 90, 90, 90, 90,  0,  0, 90, 90,
         75, 75, 75,  0, 75, 75, 75, 83, 75, 75, 75, 75,  0,  0, 75, 83);
 
%EQU JSR='J';
%EQU JMP='B';
%EQU BY1=256;
%EQU BY2=1;
 
%EJECT;
%INCLUDE MSA_C2;
    MEBUF='Hello, MSA6800 here (08/19/81)';
%INCLUDE MSA_C3;
    IF I<0 THEN DO;
        P->CELL.VISIT='1'B;
        P->CELL.EXEC='1'B;
        IF I=-1 THEN DO;
            IF PINCRC(MEM$,131060)->CELL.SOME AND PINCRC(MEM$,131062)->CELL.SOME
                THEN DO;
                CALL GETADRS(LOCTR,65530);
                PINCRC(MEM$,131060)->CELL.RD='1'B;
                HOW=2;
                GOTO INSTR;
            END;
            GOTO FINDIT;
        END;
        IF I=-2 THEN DO;
            IF PINCRC(MEM$,131056)->CELL.SOME AND PINCRC(MEM$,131058)->CELL.SOME
                THEN DO;
                CALL GETADRS(LOCTR,65528);
                PINCRC(MEM$,131056)->CELL.RD='1'B;
                HOW=2;
                GOTO INSTR;
            END;
            GOTO FINDIT;
        END;
    END;
%INCLUDE MSA_C4;
        SUBSTR(OUBUF,9,4)=TABLE0(I);
        IF I<96 OR I=141 /* BSR */ THEN DO;
            IF I/16=2 OR I=141 /* BSR */ THEN DO;
                CALL NEXTONE(I);
                IF I>127 THEN
                    I=I-256;
                I=MOD(LOCTR+1+I,65536);
                CALL CVS(K,I);
            END;
            GOTO NEXTADRS;
        END;
        IF I=140 /* CPX */ OR I=142 /* LDS */ OR I=206 /* LDX */ THEN DO;
            SUBSTR(OUBUF,18,2)='#$';
            K=20;
            CALL NEXTTWO(I);
            CALL PUTADRS(K,I);
            GOTO NEXTADRS;
        END;
        DO CASE(MOD(I/16,4));
        CASE(0); /* Immediate addressing */
            SUBSTR(OUBUF,18,2)='#$';
            K=20;
            CALL NEXTONE(I);
            CALL PUTBYTE(K,I);
            GOTO NEXTADRS;
        CASE(1); /* Direct addressing */
            CALL PG0REF(K);
        CASE(2); /* Indexed addressing */
            CALL PG0REF(K);
            SUBSTR(OUBUF,K,2)=',X';
            GOTO NEXTADRS;
        CASE(3); /* Extended addressing */
            CALL MEMREF(K);
            GOTO NEXTADRS;
        END;
%INCLUDE MSA_C5;
END;
%EOD;
/*T***********************************************************/
/*T*                                                         */
/*T* Copyright (c) Bull HN Information Systems Inc., 1989    */
/*T*                                                         */
/*T***********************************************************/
BREAK: PROC ASYNC;
 
%INCLUDE CP_6;
%FPT_TRMPRG(RSTBRK=YES);
 
    DCL BROKEN STATIC SYMDEF           SBIN;
 
    BROKEN=BROKEN+1;
    CALL M$TRMPRG(FPT_TRMPRG);
    RETURN;
END;

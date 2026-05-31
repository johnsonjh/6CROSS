/* msa8748 - Intel 8748 (MCS-48) reverse-assembler decode, a C port of
 * MSA8748_SI6 (the per-CPU half).  Links with msa_engine.c.  The mnemonic
 * table carries operand templates with placeholder letters: 'a' = 3/8 page
 * address, 's' = same-page address, 'c' = one-byte immediate. */
#include <stdio.h>
#include <string.h>
#include "msa.h"

static const char *T0[256] = {
 "NOP","..","OUTL BUS,A","ADD A,c","JMP a","EN I","..","DEC A",
 "INS A,BUS","IN A,P1","IN A,P2","..","MOVD A,P4","MOVD A,P5","MOVD A,P6","MOV A,P7",
 "INC M0","INC M1","JB0 s","ADDC A,c","CALL a","DIS I","JTF s","INC A",
 "INC R0","INC R1","INC R2","INC R3","INC R4","INC R5","INC R6","INC R7",
 "XCH A,M0","XCH A,M1","..","MOV A,c","JMP a","EN TCNTI","JNT0 s","CLR A",
 "XCHG A,R0","XCHG A,R1","XCHG A,R2","XCHG A,R3","XCHG A,R4","XCHG A,R5","XCHG A,R6","XCHG A,R7",
 "XCHD A,M0","XCHD A,M1","JB1 s","..","CALL a","DIS TCNTI","JT0 s","CPL A",
 "..","OUTL P1,A","OUTL P2,A","..","MOVD P0,A","MOVD P1,A","MOVD P2,A","MOVD P3,A",
 "ORL A,M0","ORL A,M1","MOV A,T","ORL A,c","JMP a","STRT CNT","JNT1 s","SWAP A",
 "ORL A,R0","ORL A,R1","ORL A,R2","ORL A,R3","ORL A,R4","ORL A,R5","ORL A,R6","ORL A,R7",
 "ANL A,M0","ANL A,M1","JB2 s","ANL A,c","CALL a","STRT T","JT1 s","DA A",
 "ANL A,R0","ANL A,R1","ANL A,R2","ANL A,R3","ANL A,R4","ANL A,R5","ANL A,R6","ANL A,R7",
 "ADD A,M0","ADD A,M1","MOV T,A","..","JMP a","STOP TCNT","..","RRC A",
 "ADD A,R0","ADD A,R1","ADD A,R2","ADD A,R3","ADD A,R4","ADD A,R5","ADD A,R6","ADD A,R7",
 "ADDC A,M0","ADDC A,M1","JB3 s","..","CALL a","ENTO CLK","JF1 s","RR A",
 "ADDC A,R0","ADDC A,R1","ADDC A,R2","ADDC A,R3","ADDC A,R4","ADDC A,R5","ADDC A,R6","ADDC A,R7",
 "MOVX A,M0","MOVX A,M1","..","RET","JMP a","CLR F0","JNI s","..",
 "ORL BUS,c","ORL P1,c","ORL P2,c","..","ORLD P0,A","ORLD P1,A","ORLD P2,A","ORLD P3,A",
 "MOVX M0,A","MOVX M1,A","JB4 s","RETR","CALL a","CPL F0","JNZ s","CLR C",
 "ANL BUS,c","ANL P1,c","ANL P2,c","..","ANLD P0,A","ANLD P1,A","ANLD P2,A","ANLD P3,A",
 "MOV M0,A","MOV M1,A","..","MOVR A,M","JMP a","CLR F1","..","CPL C",
 "MOV R0,A","MOV R1,A","MOV R2,A","MOV R3,A","MOV R4,A","MOV R5,A","MOV R6,A","MOV R7,A",
 "MOV M0,c","MOV M1,c","JB5 s","JMPP @A","CALL a","CPL F1","JF0 s","..",
 "MOV R0,c","MOV R1,c","MOV R2,c","MOV R3,c","MOV R4,c","MOV R5,c","MOV R6,c","MOV R7,c",
 "..","..","..","..","JMP a","SEL RB0","JZ s","MOV A,PSW",
 "DEC R0","DEC R1","DEC R2","DEC R3","DEC R4","DEC R5","DEC R6","DEC R7",
 "XRL A,M0","XRL A,M1","JB6 s","XRL A,c","CALL a","SEL RB1","..","MOV PSW,A",
 "XRL A,R0","XRL A,R1","XRL A,R2","XRL A,R3","XRL A,R4","XRL A,R5","XRL A,R6","XRL A,R7",
 "..","..","..","MOVP3 A,M","JMP a","SEL MB0","JNC s","RL A",
 "DJNZ R0,s","DJNZ R1,s","DJNZ R2,s","DJNZ R3,s","DJNZ R4,s","DJNZ R5,s","DJNZ R6,s","DJNZ R7,s",
 "MOV A,M0","MOV A,M1","JB7 s","..","CALL a","SEL MB1","JC s","RLC A",
 "MOV A,R0","MOV A,R1","MOV A,R2","MOV A,R3","MOV A,R4","MOV A,R5","MOV A,R6","MOV A,R7"
};

struct ins decode(int a)
{
    struct ins in;
    int op = byte(a);
    const char *m = T0[op], *sp;
    char mnem[12], otmpl[20], oper[24], rep[16];
    char *p;
    int idx;
    in.len = 1; in.flow = FALLTHRU; in.target = in.rdref = in.wrref = -1; in.text[0] = 0;
    if (m[0] == '.') { sprintf(in.text, "DB       $%02X", op); return in; }
    sp = strchr(m, ' ');
    if (sp) { int n = (int)(sp - m); memcpy(mnem, m, n); mnem[n] = 0; strcpy(otmpl, sp + 1); }
    else    { strcpy(mnem, m); otmpl[0] = 0; }

    rep[0] = 0;
    if ((p = strchr(otmpl, 'a'))) {                 /* 3/8 page address (JMP/CALL) */
        int tgt = ((op >> 5) << 8) | byte(a + 1);
        in.len = 2; in.target = tgt;
        in.flow = ((op & 0x1F) == 0x14) ? CALL_ : BR_UNCOND;
        lbl(rep, tgt);
    } else if ((p = strchr(otmpl, 's'))) {          /* same-page address (branches) */
        int tgt = ((a + 1) & 0xFF00) | byte(a + 1);
        in.len = 2; in.target = tgt; in.flow = BR_COND;
        lbl(rep, tgt);
    } else if ((p = strchr(otmpl, 'c'))) {          /* one-byte immediate */
        in.len = 2; sprintf(rep, "$%02X", byte(a + 1));
    }
    if (p && rep[0]) {                              /* splice rep in place of the placeholder */
        idx = (int)(p - otmpl);
        memcpy(oper, otmpl, idx); oper[idx] = 0;
        strcat(oper, rep); strcat(oper, otmpl + idx + 1);
        strcpy(otmpl, oper);
    }
    if (op == 0x83 || op == 0x93 || op == 0xB3) in.flow = STOP;  /* RET / RETR / JMPP @A */
    if (otmpl[0]) sprintf(in.text, "%-8s %s", mnem, otmpl);
    else strcpy(in.text, mnem);
    return in;
}

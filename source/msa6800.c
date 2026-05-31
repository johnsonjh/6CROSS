/* msa6800 - Motorola 6800 reverse-assembler decode, a C port of MSA6800_SI6
 * (the per-CPU half).  Links with msa_engine.c.  The 6800 is big-endian for
 * 16-bit operands.  No 6800 assembler ships in this suite, so validation is
 * by disassembling hand-built object records and checking the mnemonics. */
#include <stdio.h>
#include <string.h>
#include "msa.h"

static const char *T0[256] = {
 "..","NOP","..","..","..","..","TAP","TPA","INX","DEX","CLV","SEV","CLC","SEC","CLI","SEI",
 "SBA","CBA","..","..","..","..","TAB","TBA","..","DAA","..","ABA","..","..","..","..",
 "BRA","..","BHI","BLS","BCC","BCS","BNE","BEQ","BVC","BVS","BPL","BMI","BGE","BLT","BGT","BLE",
 "TSX","INS","PULA","PULB","DES","TXS","PSHA","PSHB","..","RTS","..","RTI","..","..","WAI","SWI",
 "NEGA","..","..","COMA","LSRA","..","RORA","ASRA","ASLA","ROLA","DECA","..","INCA","TSTA","..","CLRA",
 "NEGB","..","..","COMB","LSRB","..","RORB","ASRB","ASLB","ROLB","DECB","..","INCB","TSTB","..","CLRB",
 "NEG","..","..","COM","LSR","..","ROR","ASR","ASL","ROL","DEC","..","INC","TST","JMP","CLR",
 "NEG","..","..","COM","LSR","..","ROR","ASR","ASL","ROL","DEC","..","INC","TST","JMP","CLR",
 "SUBA","CMPA","SBCA","..","ANDA","BITA","LDAA","..","EORA","ADCA","ORAA","ADDA","CPX","BSR","LDS","..",
 "SUBA","CMPA","SBCA","..","ANDA","BITA","LDAA","STAA","EORA","ADCA","ORAA","ADDA","CPX","..","LDS","STS",
 "SUBA","CMPA","SBCA","..","ANDA","BITA","LDAA","STAA","EORA","ADCA","ORAA","ADDA","CPX","JSR","LDS","STS",
 "SUBA","CMPA","SBCA","..","ANDA","BITA","LDAA","STAA","EORA","ADCA","ORAA","ADDA","CPX","JSR","LDS","STS",
 "SUBB","CMPB","SBCB","..","ANDB","BITB","LDAB","..","EORB","ADCB","ORAB","ADDB","..","..","LDX","..",
 "SUBB","CMPB","SBCB","..","ANDB","BITB","LDAB","STAB","EORB","ADCB","ORAB","ADDB","..","..","LDX","STX",
 "SUBB","CMPB","SBCB","..","ANDB","BITB","LDAB","STAB","EORB","ADCB","ORAB","ADDB","..","..","LDX","STX",
 "SUBB","CMPB","SBCB","..","ANDB","BITB","LDAB","STAB","EORB","ADCB","ORAB","ADDB","..","..","LDX","STX"
};

static int wbe(int a) { return (byte(a) << 8) | byte(a + 1); }   /* big-endian */

struct ins decode(int a)
{
    struct ins in;
    int op = byte(a);
    const char *m = T0[op];
    char lb[16], mm[8];
    int n;
    in.len = 1; in.flow = FALLTHRU; in.target = in.rdref = in.wrref = -1; in.text[0] = 0;
    if (m[0] == '.') { sprintf(in.text, "DB       $%02X", op); return in; }
    for (n = 0; m[n] && m[n] != ' '; n++) ;
    memcpy(mm, m, n); mm[n] = 0;

    if ((op >= 0x20 && op <= 0x2F && op != 0x21) || op == 0x8D) {   /* rel branch / BSR */
        int d = byte(a + 1);
        in.len = 2; in.target = (a + 2 + (signed char)d) & 0xFFFF;
        in.flow = (op == 0x20) ? BR_UNCOND : (op == 0x8D) ? CALL_ : BR_COND;
        lbl(lb, in.target); sprintf(in.text, "%-8s %s", mm, lb); return in;
    }
    if (op < 0x60) {                                                /* inherent */
        if (op == 0x39 || op == 0x3B || op == 0x3E || op == 0x3F) in.flow = STOP; /* RTS RTI WAI SWI */
        strcpy(in.text, mm); return in;
    }
    if (op == 0x8C || op == 0x8E || op == 0xCE) {                   /* CPX/LDS/LDX immediate (16-bit) */
        in.len = 3; sprintf(in.text, "%-8s #$%04X", mm, wbe(a + 1)); return in;
    }
    {
        int mode = (op >> 4) & 3;       /* 0=imm 1=direct 2=indexed 3=extended */
        int st = (mm[0] == 'S' && mm[1] == 'T');
        switch (mode) {
        case 0: in.len = 2; sprintf(in.text, "%-8s #$%02X", mm, byte(a + 1)); break;
        case 1: in.len = 2; { int z = byte(a+1); lbl(lb,z); sprintf(in.text, "%-8s %s", mm, lb);
                              if (st) in.wrref = z; else in.rdref = z; } break;
        case 2: in.len = 2; { int z = byte(a+1); lbl(lb,z); sprintf(in.text, "%-8s %s,X", mm, lb);
                              if (st) in.wrref = z; else in.rdref = z; } break;
        case 3: in.len = 3; { int w = wbe(a+1); lbl(lb,w); sprintf(in.text, "%-8s %s", mm, lb);
                              if (st) in.wrref = w; else in.rdref = w;
                              if (op == 0x7E) { in.flow = BR_UNCOND; in.target = w; }
                              else if (op == 0xBD) { in.flow = CALL_; in.target = w; } } break;
        }
        if (op == 0x6E) in.flow = STOP;     /* JMP indexed: target unknown */
    }
    return in;
}

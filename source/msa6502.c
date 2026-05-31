/* msa6502 - 6502 reverse-assembler decode (the per-CPU half), a C port of
 * the PL/6 MSA6502_SI6.  Links with msa_engine.c (shared load/trace/emit).
 * asm6502 -> .obj -> msa6502 -> source -> asm6502 round-trips. */
#include <stdio.h>
#include <string.h>
#include "msa.h"

static const char *MRG[8] = {"ORA","AND","EOR","ADC","STA","LDA","CMP","SBC"};
static const char *BRC[8] = {"BPL","BMI","BVC","BVS","BCC","BCS","BNE","BEQ"};
static const char *SP3[8] = {"ASL","ROL","LSR","ROR","STX","LDX","DEC","INC"};
static const char *SP2[4] = {"STY","LDY","CPY","CPX"};

struct ins decode(int a)
{
    struct ins in;
    int op = byte(a), cc = op & 3, bbb = (op >> 2) & 7, aaa = (op >> 5) & 7;
    char lb[16];
    in.len = 1; in.flow = FALLTHRU; in.target = in.rdref = in.wrref = -1; in.text[0] = 0;

    switch (op) {                                   /* implied / control / transfer */
    case 0x00: strcpy(in.text,"BRK"); in.flow=STOP; return in;
    case 0x40: strcpy(in.text,"RTI"); in.flow=STOP; return in;
    case 0x60: strcpy(in.text,"RTS"); in.flow=STOP; return in;
    case 0x08: strcpy(in.text,"PHP"); return in;
    case 0x28: strcpy(in.text,"PLP"); return in;
    case 0x48: strcpy(in.text,"PHA"); return in;
    case 0x68: strcpy(in.text,"PLA"); return in;
    case 0x18: strcpy(in.text,"CLC"); return in;
    case 0x38: strcpy(in.text,"SEC"); return in;
    case 0x58: strcpy(in.text,"CLI"); return in;
    case 0x78: strcpy(in.text,"SEI"); return in;
    case 0xB8: strcpy(in.text,"CLV"); return in;
    case 0xD8: strcpy(in.text,"CLD"); return in;
    case 0xF8: strcpy(in.text,"SED"); return in;
    case 0x88: strcpy(in.text,"DEY"); return in;
    case 0xA8: strcpy(in.text,"TAY"); return in;
    case 0xC8: strcpy(in.text,"INY"); return in;
    case 0xE8: strcpy(in.text,"INX"); return in;
    case 0x8A: strcpy(in.text,"TXA"); return in;
    case 0x98: strcpy(in.text,"TYA"); return in;
    case 0x9A: strcpy(in.text,"TXS"); return in;
    case 0xAA: strcpy(in.text,"TAX"); return in;
    case 0xBA: strcpy(in.text,"TSX"); return in;
    case 0xCA: strcpy(in.text,"DEX"); return in;
    case 0xEA: strcpy(in.text,"NOP"); return in;
    case 0x20: in.len=3; in.target=word(a+1); in.flow=CALL_;     lbl(lb,in.target); sprintf(in.text,"JSR      %s",lb); return in;
    case 0x4C: in.len=3; in.target=word(a+1); in.flow=BR_UNCOND; lbl(lb,in.target); sprintf(in.text,"JMP      %s",lb); return in;
    case 0x6C: in.len=3; in.flow=STOP; lbl(lb,word(a+1)); sprintf(in.text,"JMP      (%s)",lb); return in;
    case 0x24: in.len=2; in.rdref=byte(a+1); lbl(lb,byte(a+1)); sprintf(in.text,"BIT      %s",lb); return in;
    case 0x2C: in.len=3; in.rdref=word(a+1); lbl(lb,word(a+1)); sprintf(in.text,"BIT      %s",lb); return in;
    }
    if (cc == 0 && bbb == 4) {                      /* Bcc rel */
        int d = byte(a+1); in.len = 2; in.target = (a + 2 + (signed char)d) & 0xFFFF;
        in.flow = BR_COND; lbl(lb, in.target); sprintf(in.text, "%-8s %s", BRC[aaa], lb); return in;
    }
    if (cc == 1) {                                  /* ORA/AND/EOR/ADC/STA/LDA/CMP/SBC */
        const char *m = MRG[aaa]; int w = (aaa == 4);   /* STA writes */
        switch (bbb) {
        case 0: in.len=2; { int z=byte(a+1); if(w)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s (%s,X)",m,lb);} break;
        case 1: in.len=2; { int z=byte(a+1); if(w)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 2: in.len=2; sprintf(in.text,"%-8s #$%02X",m,byte(a+1)); break;
        case 3: in.len=3; { int q=word(a+1); if(w)in.wrref=q; else in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 4: in.len=2; { int z=byte(a+1); if(w)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s (%s),Y",m,lb);} break;
        case 5: in.len=2; { int z=byte(a+1); if(w)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s,X",m,lb);} break;
        case 6: in.len=3; { int q=word(a+1); if(w)in.wrref=q; else in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s,Y",m,lb);} break;
        case 7: in.len=3; { int q=word(a+1); if(w)in.wrref=q; else in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s,X",m,lb);} break;
        }
        return in;
    }
    if (cc == 2) {                                  /* ASL/ROL/LSR/ROR/STX/LDX/DEC/INC */
        const char *m = SP3[aaa]; int st = (aaa == 4); /* STX writes */
        switch (bbb) {
        case 0: in.len=2; sprintf(in.text,"%-8s #$%02X",m,byte(a+1)); break;   /* LDX # */
        case 1: in.len=2; { int z=byte(a+1); if(st)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 2: sprintf(in.text,"%-8s A",m); break;                            /* accumulator */
        case 3: in.len=3; { int q=word(a+1); if(st)in.wrref=q; else in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 5: in.len=2; { int z=byte(a+1); const char*xy=(aaa==4||aaa==5)?",Y":",X"; if(st)in.wrref=z; else in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s%s",m,lb,xy);} break;
        case 7: in.len=3; { int q=word(a+1); const char*xy=(aaa==5)?",Y":",X"; if(st)in.wrref=q; else in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s%s",m,lb,xy);} break;
        default: sprintf(in.text,"DB       $%02X",op); break;
        }
        return in;
    }
    /* cc == 0 remaining: STY/LDY/CPY/CPX */
    {
        const char *m = SP2[(aaa >= 4) ? (aaa - 4) : 0];
        switch (bbb) {
        case 0: in.len=2; sprintf(in.text,"%-8s #$%02X",m,byte(a+1)); break;
        case 1: in.len=2; { int z=byte(a+1); in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 3: in.len=3; { int q=word(a+1); in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s",m,lb);} break;
        case 5: in.len=2; { int z=byte(a+1); in.rdref=z; lbl(lb,z); sprintf(in.text,"%-8s %s,X",m,lb);} break;
        case 7: in.len=3; { int q=word(a+1); in.rdref=q; lbl(lb,q); sprintf(in.text,"%-8s %s,X",m,lb);} break;
        default: sprintf(in.text,"DB       $%02X",op); break;
        }
    }
    return in;
}
